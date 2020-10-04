#include "pch.h"
//#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "ResourceManager.h"
#include "Model.h"
#include "PerspectiveCamera.h"
#include "LightManager.h"


void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}
using namespace Elite;
int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - **Lucas Van Baeveghem**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	std::cout << "Press F to toggle sampling technique!" << std::endl;
	std::cout << "Press R to toggle between directx and my rasterizer!" << std::endl;
	std::cout << "Press T to toggle transparency!" << std::endl;
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow) };
	PerspectiveCamera camera{ Elite::FPoint3{0,5,45},45,width/height,0.01f,100.f };
	LightManager lightMang{};
	lightMang.CreateDirectionalLight(DirectionalLightData{ Elite::FVector3{.577f,-.577f,-0.577f } }, Elite::RGBColor{ 1.f,1.f,1.f }, 7.f/3.1415f);

	ResourceManager resourceMang{};
	resourceMang.AddEffect(pRenderer->GetDevice(), L"Resources/ShaderPhong.fx", Effect::ShaderType::phongShading);
	resourceMang.AddEffect(pRenderer->GetDevice(), L"Resources/TransparencyShader.fx", Effect::ShaderType::Transparency);
	resourceMang.CreateMeshByFile(pRenderer->GetDevice(), 0, "Resources/vehicle.obj", Mesh::ShaderType::phong);
	resourceMang.AddDiffuseTexture(pRenderer->GetDevice(), "Resources/vehicle_diffuse.png");
	resourceMang.AddNormalTexture(pRenderer->GetDevice(), "Resources/vehicle_normal.png");
	resourceMang.AddGlossinessTexture(pRenderer->GetDevice(), "Resources/vehicle_gloss.png");
	resourceMang.AddSpecularTexture(pRenderer->GetDevice(), "Resources/vehicle_specular.png");
	resourceMang.CreateMeshByFile(pRenderer->GetDevice(), 1, "Resources/fireFX.obj", Mesh::ShaderType::transparent);
	resourceMang.AddDiffuseTexture(pRenderer->GetDevice(), "Resources/fireFX_diffuse.png");
	resourceMang.CreateMeshByFile(pRenderer->GetDevice(), 0, "Resources/fireFx.obj", Mesh::ShaderType::phong);
	std::vector<Model> models{};
	models.push_back(Model{ resourceMang.GetMesh(0), resourceMang.GetDiffuseTexture(0), resourceMang.GetNormalTexture(0),resourceMang.GetGlossinessTexture(0),resourceMang.GetSpecularTexture(0),Elite::FPoint3{0,0,1} });
	models.push_back(Model{ resourceMang.GetMesh(1), resourceMang.GetDiffuseTexture(1), Elite::FPoint3{0,0,1} });
	models.push_back(Model{ resourceMang.GetMesh(2), resourceMang.GetDiffuseTexture(1), Elite::FPoint3{0,0,1} });

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;

	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				break;
			}
		}

		//--------- Render ---------
		camera.Update();
		for (int i{ 0 }; i < models.size(); ++i)
		{
			models[i].Update();
			models[i].Rotate(Elite::FVector3{ 0,1,0 }, 0.5f * pTimer->GetElapsed());
		}
		pRenderer->Render(models,camera,*lightMang.GetLights()[0]);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		
		}

	}
	pTimer->Stop();

	//Shutdown "framework"
	ShutDown(pWindow);
	return 0;
}