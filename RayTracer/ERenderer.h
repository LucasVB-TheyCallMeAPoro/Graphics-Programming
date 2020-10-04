/*=============================================================================*/
// Copyright 2017-2019 Elite Engine
// Authors: Matthieu Delaere
/*=============================================================================*/
// ERenderer.h: class that holds the surface to render to, does traverse the pixels 
// and traces the rays using a tracer
/*=============================================================================*/
#ifndef ELITE_RAYTRACING_RENDERER
#define	ELITE_RAYTRACING_RENDERER

#include <cstdint>
#include <vector>

struct SDL_Window;
struct SDL_Surface;
class SceneObject;
class PerspectiveCamera;
class LightManager;
class Material;
namespace Elite
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow, const std::vector<Material*>& materialMang, const std::vector<SceneObject*>& scene1, const std::vector<SceneObject*>& scene2, PerspectiveCamera& camera, LightManager& lightMang);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render();
		bool SaveBackbufferToImage() const;

		void ToggleScene();
	private:
		SDL_Window* m_pWindow;
		SDL_Surface* m_pFrontBuffer;
		SDL_Surface* m_pBackBuffer;
		uint32_t* m_pBackBufferPixels;
		uint32_t m_Width;
		uint32_t m_Height;

		std::vector<SceneObject*> m_pScene1;
		std::vector<SceneObject*> m_pScene2;
		PerspectiveCamera* m_pCamera;
		LightManager* m_pLightManager;
		Elite::RGBColor m_FinalColorPixel;
		std::vector<Material*> m_pMatManager;

		bool m_PrevKeystateShadows;
		bool m_PrevSceneToggle;
		bool m_ToggleShadows;
		bool m_ToggleScene;
		
	};
}

#endif