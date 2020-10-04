/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render too + DirectX initialization.
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include "Model.h"
#include "Light.h"
#include "PerspectiveCamera.h"
struct SDL_Window;
struct SDL_Surface;

#undef near
#undef far
namespace Elite
{
	class Renderer final
	{
	public:
		enum class RenderState
		{
			software = 0,
			directx = 1
		};
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;
		ID3D11Device* GetDevice() const;
		void Render(const std::vector<Model>& models, const PerspectiveCamera& camera, const Light& light);

	private:
		RenderState m_RenderState;

		SDL_Window* m_pWindow;
		uint32_t m_Width;
		uint32_t m_Height;
		SDL_Surface* m_pFrontBuffer = nullptr;
		SDL_Surface* m_pBackBuffer = nullptr;
		uint32_t* m_pBackBufferPixels = nullptr;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;
		std::vector<float> m_DepthBuffer;
		std::vector<Vertex_Output> m_TransformedVertices;
		HRESULT InitializeDirectx();

		bool m_PrevSceneToggle;
		bool m_PrevTransparencyToggle;
		bool m_RenderWithTransparency;
		void ToggleScene();
		void RenderSoftware(const Model& model, const PerspectiveCamera& camera, const Light& light);
		void CalcBoundingBox(const std::vector<Elite::FPoint4>& vertices, Elite::FPoint2& topLeft, Elite::FPoint2& bottomRight);
		void TransformVertices(const std::vector<Vertex_Input>& ogVertices, std::vector<Vertex_Output>& transformedVertices, const PerspectiveCamera& camera, const Elite::FMatrix4& modelMatrix, uint32_t width, uint32_t height);
		Elite::RGBColor PixelShadingPhong(const Elite::FVector3& vNormal, const Elite::FVector2& uv, const Elite::FVector3& tangent, const Model& model, const Light& light, const Elite::FPoint3& fragPos, const Elite::FPoint3& viewPos);
		Elite::RGBColor PixelShadingTransparent(const Model& model, const Elite::FVector2& uv, float& alpha);
		bool m_IsInitialized;
		
	};
}

#endif