#pragma once
#include <cmath>
#include "ERGBColor.h"
#include "EMath.h"

namespace BRDF
{
	Elite::RGBColor Lambert(float difReflc, const Elite::RGBColor& difColor);
	Elite::RGBColor Lambert(const Elite::RGBColor& difReflc, const Elite::RGBColor& difColor);
	inline Elite::RGBColor Lambert(float difReflc, const Elite::RGBColor& difColor)
	{
		return (difColor * difReflc) / (float)E_PI;
	}
	inline Elite::RGBColor Lambert(const Elite::RGBColor& difReflc, const Elite::RGBColor& difColor)
	{
		return (difColor * difReflc) / (float)E_PI;
	}

	Elite::RGBColor Phong(float specReflc, float exponent, const Elite::FVector3& incLight, const Elite::FVector3& outLight, const Elite::FVector3& normal);
	inline Elite::RGBColor Phong(float specReflc, float exponent, const Elite::FVector3& incLight, const Elite::FVector3& outLight, const Elite::FVector3& normal)
	{
		Elite::FVector3 l{ Elite::GetNormalized(incLight) };
		Elite::FVector3 reflect{ -l + 2 * Elite::Dot(normal, l) * normal };
		float angle{ Elite::Dot(reflect, outLight) };
		float f{ specReflc * std::pow(angle, exponent) };
		return  Elite::RGBColor{ f,f,f };
	}

	Elite::RGBColor Cook_Torrance(float normalDistribution, const Elite::RGBColor& fresnel, float geometry, const Elite::FVector3& incLight, const Elite::FVector3& outLight, const Elite::FVector3& normal);

	inline Elite::RGBColor Cook_Torrance(float normalDistribution, const Elite::RGBColor& fresnel, float geometry, const Elite::FVector3& incLight, const Elite::FVector3& outLight, const Elite::FVector3& normal)
	{
		Elite::RGBColor normalDistColor{ normalDistribution, normalDistribution ,normalDistribution };
		return (normalDistColor * fresnel * geometry) / (4 * Elite::Dot(outLight, normal) * Elite::Dot(incLight, normal));
	}
	
}