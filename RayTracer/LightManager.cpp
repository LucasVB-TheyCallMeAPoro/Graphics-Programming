#include "pch.h"
#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
	for (unsigned int i{ 0 }; i < m_Lights.size(); ++i)
	{
		delete m_Lights[i];
	}
}

void LightManager::CreatePointLight(const PointLightData& pos, const RGBColor& color, float lightIntensity)
{
	m_Lights.push_back(new Light{ pos, color, lightIntensity });
}

void LightManager::CreateDirectionalLight(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity)
{
	m_Lights.push_back(new Light{ dir, color, lightIntensity });
}

const std::vector<Light*> LightManager::GetLights() const
{
	return m_Lights;
}

std::vector<Light*>& LightManager::GetLights()
{
	return m_Lights;
}
