#pragma once
#include "EMath.h"
#include <cmath>
#include "ERGBColor.h"

namespace PBR_BRDF
{
	float NormalDistribution(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float roughness);

	inline float NormalDistribution(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float roughnessSqr)
	{
		float alphaSqr = roughnessSqr * roughnessSqr;
		float dotNormalHalf{Elite::Dot(normal, halfVector) };
		return alphaSqr / (static_cast<float>(E_PI)*((dotNormalHalf * dotNormalHalf * (alphaSqr - 1.f) + 1.f) * (dotNormalHalf * dotNormalHalf * (alphaSqr - 1.f) + 1.f)));
	}

	Elite::RGBColor Fresnel(const Elite::FVector3& halfVector, const Elite::FVector3& viewDir, const Elite::RGBColor& reflect);

	inline Elite::RGBColor Fresnel(const Elite::FVector3& halfVector, const Elite::FVector3& viewDir, const Elite::RGBColor& reflect)
	{
		const Elite::RGBColor componentSubstraction{ 1.f,1.f,1.f };
		return reflect + (componentSubstraction - reflect) * (std::pow(1.f - Elite::Dot(halfVector, viewDir), 5.f));
	}

	float GeometryFunction(const Elite::FVector3& normal, const Elite::FVector3& lightDir, float roughnessSqr);

	inline float GeometryFunction(const Elite::FVector3& normal, const Elite::FVector3& lightDir, float roughnessSqr)
	{
		float k{ (roughnessSqr + 1.f) * (roughnessSqr + 1.f) / 8 };
		float DotNormalDir{ Elite::Dot(normal,lightDir) };
		return (DotNormalDir) / (DotNormalDir * (1.f - k) + k);
	}
}