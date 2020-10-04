#include "pch.h"
#include "Light.h"
#include <iostream>


Light::Light(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity)
	:m_DirectionalLightData{ dir }
	, m_Color{ color }
	, m_Intensity{ lightIntensity }
	, m_Type{ Light::Type::Directional }
	, m_Enabled{true}
{
}

Light::Type Light::GetType() const
{
	return m_Type;
}

void Light::ToggleLight()
{
	m_Enabled = !m_Enabled;
}

Elite::FVector3 Light::GetDirection() const
{
	return m_DirectionalLightData.direction;
}

Elite::RGBColor Light::GetColor() const
{
	return m_Color;
}

float Light::GetIntensity() const
{
	return m_Intensity;
}
