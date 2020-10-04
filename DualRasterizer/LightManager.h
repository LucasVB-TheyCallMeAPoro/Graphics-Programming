#pragma once
#include "Light.h"
#include <vector>
class LightManager
{
public:
	LightManager();
	~LightManager();
	//void CreatePointLight(const PointLightData& pos, const RGBColor& color, float lightIntensity);
	void CreateDirectionalLight(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity);
	const std::vector<Light*> GetLights() const;
	std::vector<Light*>& GetLights();
private:
	std::vector<Light*> m_Lights;
};

