#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* pDevice, const Effect* pEffect, const std::vector<Vertex_Input>& vertices, const std::vector<unsigned int>& indices, ShaderType shaderType)
	:m_pEffect{pEffect}
	,m_pIndexBuffer{nullptr}
	,m_pVertexBuffer{nullptr}
	, m_pVertexLayout{}
	, m_AmountIndices{}
	,m_DrawTechnique{Mesh::DrawTechnique::point}
	,m_PrevKeyStateSampleToggle{false}
	, m_Topology{Mesh::PrimitiveTopology::TriangleList}
	,m_VertexBuffer{vertices}
	,m_IndexBuffer{indices}
	,m_ShaderType{shaderType}
{
	//Create Vertex Layout
	HRESULT result = S_OK;
	static const uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};
	
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	
	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	
	//Create vertex buffer
	CD3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_Input) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//Create the input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePoint()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);
	if (FAILED(result))
		return;

	//Create index buffer
	m_AmountIndices = (uint32_t)indices.size();
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;
	for (auto& v : m_VertexBuffer)
	{
		v.normal.z = -v.normal.z;
		v.tangent = -v.tangent;
	}

}


Mesh::~Mesh()
{	
	if(m_pVertexBuffer)
		m_pVertexBuffer->Release();
	if(m_pIndexBuffer)
		m_pIndexBuffer->Release();
	if(m_pVertexLayout)
		m_pVertexLayout->Release();
}	

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Elite::FMatrix4& mWorldViewProjection, Elite::FMatrix4& mWorld, Elite::FVector3& cameraPos, Texture* diffuseMap, Texture* normalMap, Texture* glossinessMap, Texture* specularMap) const
{
	//Set vertex buffer
	UINT stride = sizeof(Vertex_Input);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//set the input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//set Matrices + cameraPos
	m_pEffect->SetMatrices(mWorldViewProjection,mWorld);
	m_pEffect->SetVector(cameraPos);
	//set TextureResourceView
	m_pEffect->SetDiffuseMap(*diffuseMap->GetShaderResource());
	if(normalMap != nullptr)
		m_pEffect->SetNormalMap(*normalMap->GetShaderResource());
	if(glossinessMap != nullptr)
		m_pEffect->SetGlossinessMap(*glossinessMap->GetShaderResource());
	if (specularMap != nullptr)
		m_pEffect->SetSpecualMap(*specularMap->GetShaderResource());

	//Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	switch (m_DrawTechnique)
	{
	case Mesh::DrawTechnique::linear:
		m_pEffect->GetTechniqueLinear()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniqueLinear()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
		}
		break;
	case  Mesh::DrawTechnique::anisotropic:
		m_pEffect->GetTechniqueAnisotropic()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniqueAnisotropic()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
		}
		break;
	default: // (point)
		m_pEffect->GetTechniquePoint()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pEffect->GetTechniquePoint()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
		}
		break;
	}
	

}

const Effect* Mesh::GetEffect() const
{
	return m_pEffect;
}

Mesh::PrimitiveTopology Mesh::GetTopology() const
{
	return m_Topology;
}

void Mesh::ToggleSampling()
{
	const Uint8* curkeystate{ SDL_GetKeyboardState(nullptr) };
	if (m_PrevKeyStateSampleToggle && !curkeystate[SDL_SCANCODE_F])
	{
		if ((int)m_DrawTechnique + 1 > 2)
			m_DrawTechnique = Mesh::DrawTechnique::point;
		else
		{
			m_DrawTechnique = static_cast<Mesh::DrawTechnique>((int)m_DrawTechnique + 1);
		}
		switch (m_DrawTechnique)
		{
		case Mesh::DrawTechnique::point:
			std::cout << "Sample technique = point" << std::endl;
			break;
		case Mesh::DrawTechnique::linear:
			std::cout << "Sample technique = linear" << std::endl;
			break;
		case Mesh::DrawTechnique::anisotropic:
			std::cout << "Sample technique = anisotropic" << std::endl;
			break;
		}
	}
	m_PrevKeyStateSampleToggle = curkeystate[SDL_SCANCODE_F];
}

Mesh::ShaderType Mesh::GetShaderType() const
{
	return m_ShaderType;
}

const std::vector<Vertex_Input>& Mesh::GetVertices() const
{
	return m_VertexBuffer;
}

const std::vector<unsigned int>& Mesh::GetIndices() const
{
	return m_IndexBuffer;
}
