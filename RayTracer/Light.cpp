#include "pch.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include <iostream>
Light::Light(const PointLightData& pos, const RGBColor& color, float lightIntensity)
	:m_PointLightData{pos}
	,m_Color{color}
	,m_Intensity{lightIntensity}
	,m_Type{ Light::Type::Point }
	,m_Enabled{true}
	,m_Toggle{Light::Toggle::All}
{
}

Light::Light(const DirectionalLightData& dir, const RGBColor& color, float lightIntensity)
	:m_DirectionalLightData{ dir }
	, m_Color{ color }
	, m_Intensity{ lightIntensity }
	, m_Type{ Light::Type::Directional }
	, m_Enabled{true}
	, m_Toggle{ Light::Toggle::All }
{
}

RGBColor Light::CalculatedBiradiance(const HitRecord& hit) const
{
	Elite::RGBColor biradiance{};
	if (!m_Enabled) return biradiance;
	
	switch (m_Type)
	{
	case Light::Type::Point:
	{
		float dot{ -Dot(hit.normal, Elite::GetNormalized(hit.hitPoint - m_PointLightData.position)) };
		if (dot < 0)
		{
			return biradiance;
		}
		switch (m_Toggle)
		{
		case Light::Toggle::Irrandiance:
			biradiance = m_Color * hit.pMaterial->Shade(hit, Elite::GetNormalized(m_PointLightData.position - hit.hitPoint), -hit.direction)* dot;
			break;
		case Light::Toggle::BRDF:
			biradiance = m_Color * (m_Intensity / Elite::SqrMagnitude(m_PointLightData.position - hit.hitPoint)) * dot;
			break;
		 default:
			biradiance = m_Color * (m_Intensity / Elite::SqrMagnitude(m_PointLightData.position - hit.hitPoint)) * hit.pMaterial->Shade(hit, Elite::GetNormalized(m_PointLightData.position - hit.hitPoint), -hit.direction) * dot;
			break;
		}
	
	}
		break;
	case Light::Type::Directional:
	{
		float dot{ -Dot(hit.normal, m_DirectionalLightData.direction) };
		if (dot < 0)
		{
			return biradiance;
		}
		switch (m_Toggle)
		{
		case Light::Toggle::Irrandiance:
			biradiance = m_Color * hit.pMaterial->Shade(hit, -m_DirectionalLightData.direction, -hit.direction) * dot;
			break;
		case Light::Toggle::BRDF:
			biradiance = m_Color * m_Intensity * dot;
			break;
		default:
			biradiance = m_Color * m_Intensity * hit.pMaterial->Shade(hit, -m_DirectionalLightData.direction, -hit.direction) * dot;
			break;
		}
	}
		break;
	}
	return biradiance;
}

Light::Type Light::GetType() const
{
	return m_Type;
}

void Light::ToggleLight()
{
	m_Enabled = !m_Enabled;
}

void Light::ToggleCalculation() 
{
	int toggle{ (int)m_Toggle };
	toggle += 1;
	if (toggle > (int)Light::Toggle::All)
	{
		m_Toggle = Light::Toggle::Irrandiance;
	}
	else
	{
		m_Toggle = static_cast<Light::Toggle>(toggle);
	}
}

Elite::FVector3 Light::GetDirection(const HitRecord& hit) const
{
	switch (m_Type)
	{
	case Light::Type::Point:
		return hit.hitPoint - m_PointLightData.position;
		break;
	case Light::Type::Directional:
		return m_DirectionalLightData.direction;
		break;
	}
}
