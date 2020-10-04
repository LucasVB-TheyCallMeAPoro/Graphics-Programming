#include "pch.h"
//Project includes
#include "ERenderer.h"
#include "Mesh.h"
#include "EMath.h"
#include "ScreenBuffer.h"
#include <vector>
using namespace Elite;
inline Elite::FPoint4 operator+(const Elite::FPoint4& lhs, const Elite::FPoint4& rhs)
{

	return Elite::FPoint4{ lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z,1.f };
}
Elite::Renderer::Renderer(SDL_Window * pWindow)
	: m_pWindow{ pWindow }
	, m_Width{}
	, m_Height{}
	, m_IsInitialized{ false }
	,m_pSwapChain{nullptr}
	,m_pRenderTargetBuffer{nullptr}
	,m_pRenderTargetView{nullptr}
	,m_pDevice{nullptr}
	,m_pDeviceContext{nullptr}
	,m_pDepthStencilBuffer{nullptr}
	,m_pDepthStencilView{nullptr}
	,m_pDXGIFactory{nullptr}
	, m_RenderState{Renderer::RenderState::directx}
	, m_PrevSceneToggle{ false }
	,m_RenderWithTransparency{true}
{
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	//Initialize
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
	m_DepthBuffer.resize(static_cast<size_t>(width)* static_cast<size_t>(height));
	//Initialize DirectX pipeline
	InitializeDirectx();

	m_IsInitialized = true;
	std::cout << "DirectX is ready\n";
}

Elite::Renderer::~Renderer()
{
	if (m_pDeviceContext)
	{
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();
	}
	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if(m_pRenderTargetBuffer)
		m_pRenderTargetBuffer->Release();
	if(m_pDepthStencilView)
		m_pDepthStencilView->Release();
	if(m_pDepthStencilBuffer)
		m_pDepthStencilBuffer->Release();
	if(m_pSwapChain)
		m_pSwapChain->Release(); 
	if(m_pDevice)
		m_pDevice->Release();
	if(m_pDXGIFactory)
		m_pDXGIFactory->Release();
}

ID3D11Device* Elite::Renderer::GetDevice() const
{
	return m_pDevice;
}

void Elite::Renderer::Render(const std::vector<Model>& models, const PerspectiveCamera& camera, const Light& light)
{
	ToggleScene();
	switch (m_RenderState)
	{
	case Elite::Renderer::RenderState::software:
		std::fill_n(m_DepthBuffer.data(), m_Width * m_Height, INFINITY);
		SDL_LockSurface(m_pBackBuffer);
		std::fill_n(m_pBackBufferPixels, m_Width * m_Height, SDL_MapRGB(m_pBackBuffer->format, 0.f, 0.f,0.3f*255.f));
		for (int i{ 0 }; i < models.size(); ++i)
		{
			if (m_RenderWithTransparency && i == 2)
				continue;
			else if (!m_RenderWithTransparency && i == 1)
				continue;
			RenderSoftware(models[i], camera, light);
		}
		break;
	case Elite::Renderer::RenderState::directx:
		if (!m_IsInitialized)
			return;
		//Clear Buffers
		RGBColor clearColor = RGBColor(0.f, 0.f, 0.3f);
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//Render
		for (int i{ 0 }; i < models.size(); ++i)
		{
			if (m_RenderWithTransparency && i == 2)
				continue;
			else if (!m_RenderWithTransparency && i == 1)
				continue;
			models[i].Render(m_pDeviceContext, camera);
		}

		//Present
		m_pSwapChain->Present(1, 0);
		break;
	}
	
}

HRESULT Elite::Renderer::InitializeDirectx()
{
	//Create Device and Device context, using hardware acceleration
	D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 };
	uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
	if (FAILED(result))
		return result;
	//Create DXGI Factory to create SwapChain based on hardware
	result = CreateDXGIFactory(_uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
	if (FAILED(result))
		return result;

	//Create SwapChein Descriptor
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Get the handle HWND from the SDL backbuffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	//Create SwapChain and hook it into the handle of the SDL window
	result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
		return result;

	//Create the Depth/Stencil Buffer and View
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(result))
		return result;
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
		return result;

	//Create the RenderTrargetView
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
		return result;
	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
	if (FAILED(result))
		return result;

	//Bind the Views to the Output Merger Stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//Set the viewport
	D3D11_VIEWPORT viewPort{};
	viewPort.Width = static_cast<float>(m_Width);
	viewPort.Height = static_cast<float>(m_Height);
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	viewPort.MinDepth = 0.f;
	viewPort.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewPort);
	
	return result;
}

void Elite::Renderer::ToggleScene()
{
	const Uint8* curkeystate{ SDL_GetKeyboardState(nullptr) };
	if (m_PrevSceneToggle && !curkeystate[SDL_SCANCODE_R])
	{
		if ((int)m_RenderState + 1 > 1)
			m_RenderState = Renderer::RenderState::software;
		else
			m_RenderState = Renderer::RenderState::directx;

		switch (m_RenderState)
		{
		case Elite::Renderer::RenderState::software:
			std::cout << "Rendering my rasterizer!" << std::endl;
			break;
		case Elite::Renderer::RenderState::directx:
			std::cout << "Rendering using directx!" << std::endl;
			break;
		}
	}
	m_PrevSceneToggle = curkeystate[SDL_SCANCODE_R];
	if(m_PrevTransparencyToggle && !curkeystate[SDL_SCANCODE_T])
	{
		m_RenderWithTransparency = !m_RenderWithTransparency;
	}
	m_PrevTransparencyToggle = curkeystate[SDL_SCANCODE_T];
}

void Elite::Renderer::RenderSoftware(const Model& model, const PerspectiveCamera& camera, const Light& light)
{
	ScreenBuffer screenBuffer{m_pBackBuffer};
	
	m_TransformedVertices.reserve(model.GetMesh()->GetVertices().size());
	m_TransformedVertices.clear();
	//ProjectionStage
	TransformVertices(model.GetMesh()->GetVertices(), m_TransformedVertices, camera , model.GetTransform(), m_Width, m_Height);

	for (unsigned int i{ 0 }; i < model.GetMesh()->GetIndices().size() - 2; i += 3)
	{
		unsigned int v0{};
		unsigned int v1{};
		unsigned int v2{};
		int iPlusOne{ static_cast<int>(i) + 1 };
		int iPlusTwo{ static_cast<int>(i) + 2 };

		//Creating triangle based on PrimitiveTopology
		switch (model.GetMesh()->GetTopology())
		{
		case Mesh::PrimitiveTopology::TriangleList:
			v0 = model.GetMesh()->GetIndices()[i];
			v1 = model.GetMesh()->GetIndices()[iPlusOne];
			v2 = model.GetMesh()->GetIndices()[iPlusTwo];
			break;
		case Mesh::PrimitiveTopology::TriangleStrip:
			v0 = model.GetMesh()->GetIndices()[i];
			v1 = (i % 2) ? model.GetMesh()->GetIndices()[iPlusTwo] : model.GetMesh()->GetIndices()[iPlusOne];
			v2 = (i % 2) ? model.GetMesh()->GetIndices()[iPlusOne] : model.GetMesh()->GetIndices()[iPlusTwo];
			break;
		}
		//frustrum culling
		if (m_TransformedVertices[v0].position.w > camera.GetFar() || m_TransformedVertices[v0].position.w < camera.GetNear() || m_TransformedVertices[v2].position.w > camera.GetFar() || m_TransformedVertices[v2].position.w < camera.GetNear() || m_TransformedVertices[v1].position.w > camera.GetFar() || m_TransformedVertices[v1].position.w < camera.GetNear())
			continue;

		//BoundingBox check of each triangle
		Elite::FPoint2 boundingBoxTopLeft;
		Elite::FPoint2 boundingBoxBottomRight;
		std::vector<Elite::FPoint4> faceVertex{ m_TransformedVertices[v0].position,m_TransformedVertices[v1].position,m_TransformedVertices[v2].position };

		CalcBoundingBox(faceVertex, boundingBoxTopLeft, boundingBoxBottomRight);
		if (boundingBoxBottomRight.x <= 0 || boundingBoxTopLeft.x >= m_Width)
			continue;
		if (boundingBoxBottomRight.y <= 0 || boundingBoxTopLeft.y >= m_Height)
			continue;
		if (boundingBoxBottomRight.y >= m_Height)
		{
			boundingBoxBottomRight.y = static_cast<float>(m_Height - 1);
		}
		if (boundingBoxBottomRight.x >= m_Width)
		{
			boundingBoxBottomRight.x = static_cast<float>(m_Width - 1);
		}
		for (uint32_t r = static_cast<uint32_t>(boundingBoxTopLeft.y); r < boundingBoxBottomRight.y; ++r)
		{
			for (uint32_t c = static_cast<uint32_t>(boundingBoxTopLeft.x); c < boundingBoxBottomRight.x; ++c)
			{
				float cPos = static_cast<float>(c);
				float rPos = static_cast<float>(r);


				//Doing the bary centric coord check
				Elite::FPoint2 pixelCoord{ cPos,rPos };
				Elite::FVector2 verToPixV0{ pixelCoord - static_cast<Elite::FPoint2>(m_TransformedVertices[v0].position) };
				Elite::FVector2 v0Tov1{ m_TransformedVertices[v1].position - m_TransformedVertices[v0].position };
				float bc[3];
				bc[0] = Elite::Cross(verToPixV0, v0Tov1);
				if (bc[0] <= 0)
				{
					continue;
				}
				Elite::FVector2 verToPixV1{ pixelCoord - static_cast<Elite::FPoint2>(m_TransformedVertices[v1].position) };
				Elite::FVector2 v1Tov2{ m_TransformedVertices[v2].position - m_TransformedVertices[v1].position };
				bc[1] = Elite::Cross(verToPixV1, v1Tov2);
				if (bc[1] <= 0)
				{
					continue;
				}
				Elite::FVector2 verToPixV2{ pixelCoord - static_cast<Elite::FPoint2>(m_TransformedVertices[v2].position) };
				Elite::FVector2 v2Tov0{ m_TransformedVertices[v0].position - m_TransformedVertices[v2].position };
				bc[2] = Elite::Cross(verToPixV2, v2Tov0);
				if (bc[2] <= 0)
				{
					continue;
				}

				//Getting the weight of the vertices
				float totalAreaFace{ Elite::Cross(verToPixV0 - verToPixV1, verToPixV0 - verToPixV2) };
				float w0{ -bc[1] / totalAreaFace };
				float w1{ -bc[2] / totalAreaFace };
				float w2{ -bc[0] / totalAreaFace };

				//depth buffer check (not linear)
				float zComp{ 1 / ((1 / m_TransformedVertices[v0].position.z) * w0 + (1 / m_TransformedVertices[v1].position.z) * w1 + (1 / m_TransformedVertices[v2].position.z) * w2) };
				if (zComp < 0.f || zComp >1.f || m_DepthBuffer[c + (r * static_cast<size_t>(m_Width))] < zComp)
				{
					continue;
				}
				if(model.GetMesh()->GetShaderType() != Mesh::ShaderType::transparent)
					m_DepthBuffer[c + (r * static_cast<size_t>(m_Width))] = zComp;

				//interpolating the w component en vertex uvs
				float wInterpolated{ 1 / ((1 / m_TransformedVertices[v0].position.w) * w0 + (1 / m_TransformedVertices[v1].position.w) * w1 + (1 / m_TransformedVertices[v2].position.w) * w2) };
				Elite::FVector2 uvV0 = (model.GetMesh()->GetVertices()[v0].UV / m_TransformedVertices[v0].position.w) * w0;
				Elite::FVector2 uvV1 = (model.GetMesh()->GetVertices()[v1].UV / m_TransformedVertices[v1].position.w) * w1;
				Elite::FVector2 uvV2 = (model.GetMesh()->GetVertices()[v2].UV / m_TransformedVertices[v2].position.w) * w2;
				Elite::FVector2 uv{ (uvV0 + uvV1 + uvV2) * wInterpolated };

				//Calculating the finalcolor
				Elite::RGBColor finalColor{};
				Elite::FPoint4 interpolatedPosition = (((m_TransformedVertices[v0].position / m_TransformedVertices[v0].position.w) * w0) + ((m_TransformedVertices[v1].position / m_TransformedVertices[v1].position.w) * w1) + ((m_TransformedVertices[v2].position / m_TransformedVertices[v2].position.w) * w2)) * wInterpolated;
				Elite::FVector3 interpolatedNormal = (((m_TransformedVertices[v0].normal / m_TransformedVertices[v0].position.w) * w0) + ((m_TransformedVertices[v1].normal / m_TransformedVertices[v1].position.w) * w1) + ((m_TransformedVertices[v2].normal / m_TransformedVertices[v2].position.w) * w2)) * wInterpolated;
				Elite::FVector3 interpolatedTangent = (((m_TransformedVertices[v0].tangent / m_TransformedVertices[v0].position.w) * w0) + ((m_TransformedVertices[v1].tangent / m_TransformedVertices[v1].position.w) * w1) + ((m_TransformedVertices[v2].tangent / m_TransformedVertices[v2].position.w) * w2)) * wInterpolated;
				Elite::FPoint3 viewPos{ Elite::Inverse(static_cast<Elite::FMatrix3>(camera.GetViewMatrix())) * -static_cast<Elite::FVector3>(camera.GetViewMatrix()[3]) };
				switch (model.GetMesh()->GetShaderType())	
				{
				case Mesh::ShaderType::phong:
					finalColor = PixelShadingPhong(interpolatedNormal, uv, interpolatedTangent, model, light, static_cast<Elite::FPoint3>(interpolatedPosition), viewPos);
					finalColor.MaxToOne();
					screenBuffer.SetPixel(r, c, finalColor);
					break;
				case Mesh::ShaderType::transparent:
					float alpha;
					finalColor = PixelShadingTransparent(model, uv, alpha);
					finalColor.Clamp();
					screenBuffer.SetPixel(r, c, finalColor, alpha);
					break;
				}
			}
		}
	}
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Elite::Renderer::CalcBoundingBox(const std::vector<Elite::FPoint4>& vertices, Elite::FPoint2& topLeft, Elite::FPoint2& bottomRight)
{
	topLeft.x = INFINITY;
	topLeft.y = INFINITY;
	bottomRight.x = -INFINITY;
	bottomRight.y = -INFINITY;
	for (int i{ 0 }; i < vertices.size(); ++i)
	{
		if (vertices[i].x < topLeft.x) topLeft.x = vertices[i].x;
		if (vertices[i].y < topLeft.y) topLeft.y = vertices[i].y;
		if (vertices[i].x > bottomRight.x) bottomRight.x = vertices[i].x;
		if (vertices[i].y > bottomRight.y) bottomRight.y = vertices[i].y;
	}
}

void Elite::Renderer::TransformVertices(const std::vector<Vertex_Input>& ogVertices, std::vector<Vertex_Output>& transformedVertices,const PerspectiveCamera& camera, const Elite::FMatrix4& modelMatrix, uint32_t width, uint32_t height)
{
	for (int i{ 0 }; i < ogVertices.size(); i++)
	{
		Vertex_Output vert{};
		vert.UV = ogVertices[i].UV;
		vert.normal = static_cast<Elite::FVector3>(modelMatrix * static_cast<Elite::FVector4>(ogVertices[i].normal));
		vert.tangent = static_cast<Elite::FVector3>(modelMatrix * static_cast<Elite::FVector4>(ogVertices[i].tangent));
		//Multipling position with modelmatrix to convert to worldspace
		Elite::FPoint4 rotatedPos{ modelMatrix * static_cast<Elite::FPoint4>(ogVertices[i].position) };
		//Multipling the world position with the cameramatrix (viewmatrix)
		Elite::FPoint4 viewSpacePos{ camera.GetViewMatrix() * rotatedPos };
		//postion in viewspace * projectionMatrix
		Elite::FPoint4 projectedVertex{ camera.GetProjectionMatrix() * viewSpacePos };

		//Perspective devide
		projectedVertex.x /= projectedVertex.w;
		projectedVertex.y /= projectedVertex.w;
		projectedVertex.z /= projectedVertex.w;

		//converting the x and y to screen space
		float screenSpaceVertexX{ ((projectedVertex.x + 1) / 2) * width };
		float screenSpaceVertexY{ ((1 - projectedVertex.y) / 2) * height };
		vert.position = Elite::FPoint4{ screenSpaceVertexX,screenSpaceVertexY, projectedVertex.z, projectedVertex.w };
		transformedVertices.push_back(vert);
	}
}

Elite::RGBColor Elite::Renderer::PixelShadingPhong(const Elite::FVector3& vNormal, const Elite::FVector2& uv, const Elite::FVector3& tangent, const Model& model, const Light& light, const Elite::FPoint3& fragPos, const Elite::FPoint3& viewPos)
{
	//Converting sampledValue to correct normal
	Elite::FVector3 nNormal = Elite::GetNormalized(vNormal);
	Elite::FVector3 nTangent = Elite::GetNormalized(tangent);
	Elite::FVector3 binormal = Elite::Cross(nNormal, nTangent);
	Elite::FMatrix3 tangentSpaceAxis = Elite::FMatrix3{ nTangent,binormal,nNormal };

	Elite::RGBColor sampledNormal = model.SampleTexture(uv, Model::TextureToSample::normal) * 2.f;
	Elite::FVector3 convertedNormal{ sampledNormal.r - 1.f, sampledNormal.g - 1.f, sampledNormal.b - 1.f };
	Elite::FVector3 newNormal = Elite::GetNormalized(tangentSpaceAxis * convertedNormal);

	//Phong
	Elite::RGBColor specularColor{ model.SampleTexture(uv, Model::TextureToSample::specular) };
	float exponent{ model.SampleTexture(uv, Model::TextureToSample::glossiness).r * model.GetShininess() };
	Elite::FVector3 viewDir{ Elite::GetNormalized( fragPos - viewPos) };
	Elite::FVector3 reflectDir{GetNormalized(Reflect(light.GetDirection(),newNormal))};

	//LightCalculations
	Elite::RGBColor finalcolor{};
	Elite::RGBColor diffuse{ model.SampleTexture(uv, Model::TextureToSample::diffuse) * Elite::Clamp(Elite::Dot(newNormal,-light.GetDirection()), 0.f, 1.f) };
	Elite::RGBColor specular{specularColor * std::pow(Elite::Clamp(-Elite::Dot(viewDir,reflectDir),0.f,1.f),exponent)};
	finalcolor = ( diffuse + specular) * light.GetIntensity();
	return finalcolor;
}

Elite::RGBColor Elite::Renderer::PixelShadingTransparent(const Model& model, const Elite::FVector2& uv, float& alpha)
{
	return model.SampleTextureWithTransparency(uv,alpha,Model::TextureToSample::diffuse);
}


