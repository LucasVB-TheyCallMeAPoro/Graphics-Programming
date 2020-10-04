#include "pch.h"
//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "ERenderer.h"
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "PerspectiveCamera.h"
#include "LightManager.h"
#include "Lambert_Material.h"
#include "SceneObject.h"
Elite::Renderer::Renderer(SDL_Window * pWindow, const std::vector<Material*>& materialMang, const std::vector<SceneObject*>& scene1, const std::vector<SceneObject*>& scene2,PerspectiveCamera& camera,LightManager& lightMang)
	:m_pWindow{ pWindow }
	, m_pFrontBuffer{ SDL_GetWindowSurface(pWindow) }
	, m_pBackBuffer{}
	, m_pBackBufferPixels{}
	, m_Width{}
	, m_Height{}
	, m_FinalColorPixel{0.f,0.f,0.f}
	, m_pLightManager{ &lightMang }
	, m_pMatManager{ materialMang }
	, m_pScene1{ scene1 }
	, m_pScene2{ scene2 }
	,m_pCamera{&camera}
	, m_ToggleShadows{true}
	, m_PrevKeystateShadows{}
	, m_PrevSceneToggle{}
	, m_ToggleScene{false}
{
	//Initialize
	int width, height = 0;
	SDL_GetWindowSize(pWindow, &width, &height);
	m_Width = static_cast<uint32_t>(width);
	m_Height = static_cast<uint32_t>(height);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;


	
}

Elite::Renderer::~Renderer()
{
	for (int i{}; i < m_pScene1.size(); i++)
	{
		delete m_pScene1[i];
	}
	for (int i{}; i < m_pScene2.size(); i++)
	{
		delete m_pScene2[i];
	}
}

void Elite::Renderer::Render()
{
	ToggleScene();
	SDL_LockSurface(m_pBackBuffer);

	auto lookatMatrix{ m_pCamera->CalculateMatrix() };
	std::vector<Light*> plights{m_pLightManager->GetLights()};
	//Loop over all the pixels
	for (uint32_t r = 0; r < m_Height; ++r)
	{
		for (uint32_t c = 0; c < m_Width; ++c)
		{
			Ray ray{};
			m_pCamera->CalcRay(ray, c, r, m_Width, m_Height);
			ray.direction = FVector3{  lookatMatrix * Elite::FVector4{ ray.direction } };
			HitRecord hitInfo{};
			HitRecord closestHitInfo{};
			closestHitInfo.intersection = FLT_MAX;
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format, 0, 0, 0);
			if (m_ToggleScene)
			{
				for (int i{ 0 }; i < m_pScene2.size(); i++)
				{
					if (m_pScene2[i]->Hit(ray, hitInfo) && closestHitInfo.intersection > hitInfo.intersection)
					{
						hitInfo.pMaterial = m_pScene2[i]->GetMaterial();
						closestHitInfo = hitInfo;
					}
				}
			}
			else
			{
				for (int i{ 0 }; i < m_pScene1.size(); i++)
				{
					if (m_pScene1[i]->Hit(ray, hitInfo) && closestHitInfo.intersection > hitInfo.intersection)
					{
						hitInfo.pMaterial = m_pScene1[i]->GetMaterial();
						closestHitInfo = hitInfo;
					}
				}
			}
			if (closestHitInfo.intersection == FLT_MAX) continue;
			hitInfo = closestHitInfo;
		
			Elite::RGBColor finalColor{0,0,0};
			for (int j{ 0 }; j < plights.size(); j++)
			{
				if (m_ToggleShadows)
				{
					bool hit{ false };
					Ray shadowRay{};
					HitRecord shadowInfo{};
					shadowRay.tMax = Elite::Magnitude(plights[j]->GetDirection(hitInfo));
					shadowRay.direction = -plights[j]->GetDirection(hitInfo) / shadowRay.tMax;
					shadowRay.origin = hitInfo.hitPoint;
					shadowRay.inverse = true;
					if (m_ToggleScene)
					{
						for (int k{ 0 }; k < m_pScene2.size(); k++)
						{
							if (m_pScene2[k]->Hit(shadowRay, shadowInfo))
							{
								hit = true;
							}
						}
					}
					else
					{
						for (int k{ 0 }; k < m_pScene1.size(); k++)
						{
							if (m_pScene1[k]->Hit(shadowRay, shadowInfo))
							{
								hit = true;
							}
						}
					}
					if (hit) continue;
				}
				finalColor += plights[j]->CalculatedBiradiance(hitInfo);
			}
			
			finalColor.MaxToOne();
			m_pBackBufferPixels[c + (r * m_Width)] = SDL_MapRGB(m_pBackBuffer->format, static_cast<uint8_t>(finalColor.r * 255.f), static_cast<uint8_t>(finalColor.g * 255.f), static_cast<uint8_t>(finalColor.b * 255.f));
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Elite::Renderer::SaveBackbufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "BackbufferRender.bmp");
}

void Elite::Renderer::ToggleScene()
{
	const Uint8* curkeystate{ SDL_GetKeyboardState(nullptr) };
	if (m_PrevKeystateShadows && !curkeystate[SDL_SCANCODE_Z])
	{
		m_ToggleShadows = !m_ToggleShadows;
	}
	m_PrevKeystateShadows = curkeystate[SDL_SCANCODE_Z];
	
	if (m_PrevSceneToggle && !curkeystate[SDL_SCANCODE_V])
	{
		m_ToggleScene = !m_ToggleScene;
	}
	m_PrevSceneToggle = curkeystate[SDL_SCANCODE_V];
}


