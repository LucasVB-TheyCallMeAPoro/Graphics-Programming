#include "pch.h"
//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>
#include <fstream>
//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "EMath.h"
#include "ERGBColor.h"

//Own Project includes
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "SceneObject.h"
#include "Mesh.h"
#include "TriangleObject.h"

#include "LightManager.h"
#include "Light.h"

#include "Lambert_Material.h"
#include "PhongLambert_Material.h"
#include "PBR_Material.h"

#include "PerspectiveCamera.h"
std::vector<SceneObject*> m_pScene1{};
std::vector<SceneObject*> m_pScene2{};
std::vector<Material*> m_pMatManager{};
PerspectiveCamera m_pCamera{};
LightManager m_pLightManager{};
bool m_PrevKeystateDirLight{  };
bool m_PrevKeystateLeftLight{};
bool m_PrevKeyststateRightLight{};
bool m_PrevKesystateCalc{};
void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}
void InitScene(float witdh, float height);
void ToggleScene();
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
		"RayTracer - **Lucas Van Baeveghem**",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;
	InitScene(width, height);
	

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow,m_pMatManager,m_pScene1,m_pScene2,m_pCamera,m_pLightManager);

	
	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	float angle{ 0.f };
	float rotSpeed{ 10.f };
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
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				break;
			}
		}

		ToggleScene();
		m_pCamera.Update();
		dynamic_cast<TriangleObject*>(m_pScene1[10])->RotateTriangle(pTimer->GetElapsed());
		dynamic_cast<TriangleObject*>(m_pScene1[11])->RotateTriangle(pTimer->GetElapsed());
		dynamic_cast<TriangleObject*>(m_pScene1[12])->RotateTriangle(pTimer->GetElapsed());
		//--------- Render ---------
		pRenderer->Render();
		
		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}
void InitScene(float width, float height)
{
	m_pCamera = PerspectiveCamera{ Elite::FPoint3{ 0,4,6 }, (float)E_PI / 4, width / height };

	m_pMatManager.push_back(new Lambert_Material{ 1.f,Elite::RGBColor(0.25f,0.5f,0.75f) });
	m_pMatManager.push_back(new PBR_Material{ 1.f, Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f), true });
	m_pMatManager.push_back(new PBR_Material{ 0.5f,Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f),true });
	m_pMatManager.push_back(new PBR_Material{ 0.1f,Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f),true });
	m_pMatManager.push_back(new PBR_Material{ 1.f,Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f),false });
	m_pMatManager.push_back(new PBR_Material{ 0.5f,Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f),false });
	m_pMatManager.push_back(new PBR_Material{ 0.1f,Elite::RGBColor(250 / 255.f,128 / 255.f,114 / 255.f),false });
	m_pMatManager.push_back(new PBR_Material{ 0.5f, Elite::RGBColor(1.000f, 0.766f, 0.336f),true });
	m_pMatManager.push_back(new Lambert_Material{ 1.f,Elite::RGBColor(0.75f,0.5f,0.25f) });


	m_pScene1.push_back(new Plane{ Elite::FPoint3(0.f,0.f,0.f), Elite::FVector3(0,1,0), *m_pMatManager[0] });
	m_pScene1.push_back(new Plane{ Elite::FPoint3(10.f,0.f,0.f),Elite::FVector3(-1,0,0), *m_pMatManager[0] });
	m_pScene1.push_back(new Plane{ Elite::FPoint3(-10.f,0.f,0.f),Elite::FVector3(1,0,0), *m_pMatManager[0] });
	m_pScene1.push_back(new Plane{ Elite::FPoint3(0.f,0.f,-10.f),Elite::FVector3(0,0,1), *m_pMatManager[0] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(-3.f,1.f,0.f), 1.f,*m_pMatManager[1] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(0.f,1.f,0.f), 1.f,*m_pMatManager[2] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(3.f,1.f,0.f), 1.f,*m_pMatManager[3] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(-3.f,4.f,0.f), 1.f,*m_pMatManager[4] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(0.f,4.f,0.f), 1.f,*m_pMatManager[5] });
	m_pScene1.push_back(new Sphere{ Elite::FPoint3(3.f,4.f,0.f), 1.f,*m_pMatManager[6] });
	m_pScene1.push_back(new TriangleObject{ Elite::FPoint3(-3.f,6.f,0.f), *m_pMatManager[8],TriangleObject::faceCulling::back_face });
	m_pScene1.push_back(new TriangleObject{ Elite::FPoint3(0.f,6.f,0.f), *m_pMatManager[8], TriangleObject::faceCulling::front_face });
	m_pScene1.push_back(new TriangleObject{ Elite::FPoint3(3.f,6.f,0.f), *m_pMatManager[8] });
	m_pScene2.push_back(new Plane{ Elite::FPoint3(0.f,0.f,0.f), Elite::FVector3(0,1,0), *m_pMatManager[0] });
	m_pScene2.push_back(new Plane{ Elite::FPoint3(10.f,0.f,0.f),Elite::FVector3(-1,0,0), *m_pMatManager[0] });
	m_pScene2.push_back(new Plane{ Elite::FPoint3(-10.f,0.f,0.f),Elite::FVector3(1,0,0), *m_pMatManager[0] });
	m_pScene2.push_back(new Plane{ Elite::FPoint3(0.f,0.f,-10.f),Elite::FVector3(0,0,1), *m_pMatManager[0] });
	m_pScene2.push_back(new Mesh{ Elite::FPoint3{0.f,0.f,0.f}, *m_pMatManager[7] });

	//TriangleObject
	dynamic_cast<TriangleObject*>(m_pScene1[10])->SetVertices(FPoint3{ -0.75f,1.5f,0.f }, FPoint3{ -0.75f,0.f,0.f }, FPoint3{ 0.75f,0.f,0.f });
	dynamic_cast<TriangleObject*>(m_pScene1[11])->SetVertices(FPoint3{ -0.75f,1.5f,0.f }, FPoint3{ -0.75f,0.f,0.f }, FPoint3{ 0.75f,0.f,0.f });
	dynamic_cast<TriangleObject*>(m_pScene1[12])->SetVertices(FPoint3{ -0.75f,1.5f,0.f }, FPoint3{ -0.75f,0.f,0.f }, FPoint3{ 0.75f,0.f,0.f });

	//MeshObject
	std::vector<std::string> coord;
	std::string filename{ "lowpoly_bunny.obj" };
	std::ifstream in(filename);
	if (!in.is_open())
	{
		std::cout << "can not open file " << filename << std::endl;
	}
	char buf[256];
	while (!in.eof())
	{
		in.getline(buf, 256);
		coord.push_back(std::string{ buf });
	}
	for (int i{ 0 }; i < coord.size(); ++i)
	{
		if (coord[i][0] == '#')
			continue;
		else if (coord[i][0] == 'v' && coord[i][1] == ' ')
		{
			float tmpx, tmpy, tmpz;
			sscanf_s(coord[i].c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);
			dynamic_cast<Mesh*>(m_pScene2[4])->AddVertex(Elite::FPoint3{ tmpx,tmpy,tmpz });
		}
		else if (coord[i][0] == 'f')
		{
			unsigned int v0, v1, v2;
			sscanf_s(coord[i].c_str(), "f %i %i %i", &v0, &v1, &v2);
			dynamic_cast<Mesh*>(m_pScene2[4])->AddTriangle(v0 - 1, v1 - 1, v2 - 1);
		}
	}

	m_pLightManager.CreatePointLight(PointLightData{ Elite::FPoint3{ 0,5,-5 } }, Elite::RGBColor{ 1,1,1 }, 50.f);
	m_pLightManager.CreatePointLight(PointLightData{ Elite::FPoint3{ 0,2.5,5 } }, Elite::RGBColor{ 1,1,1 }, 50.f);
	m_pLightManager.CreateDirectionalLight(DirectionalLightData{ Elite::GetNormalized(Elite::FVector3{1.f,-5.f,1.57f}) }, Elite::RGBColor{ 1,1,1 }, 1.f);
}
void ToggleScene()
{
	const Uint8* curkeystate{ SDL_GetKeyboardState(nullptr) };
	if (m_PrevKeystateDirLight && !curkeystate[SDL_SCANCODE_J])
	{
		m_pLightManager.GetLights()[2]->ToggleLight();
	}
	m_PrevKeystateDirLight = curkeystate[SDL_SCANCODE_J];

	if (m_PrevKeyststateRightLight && !curkeystate[SDL_SCANCODE_L])
	{
		m_pLightManager.GetLights()[0]->ToggleLight();
	}
	m_PrevKeyststateRightLight = curkeystate[SDL_SCANCODE_L];

	if (m_PrevKeystateLeftLight && !curkeystate[SDL_SCANCODE_K])
	{
		m_pLightManager.GetLights()[1]->ToggleLight();
	}
	m_PrevKeystateLeftLight = curkeystate[SDL_SCANCODE_K];

	if (m_PrevKesystateCalc && !curkeystate[SDL_SCANCODE_T])
	{
		for (int i{ 0 }; i < m_pLightManager.GetLights().size(); ++i)
		{
			m_pLightManager.GetLights()[i]->ToggleCalculation();
		}
	}
	m_PrevKesystateCalc = curkeystate[SDL_SCANCODE_T];
}
