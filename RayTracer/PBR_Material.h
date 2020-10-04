#pragma once
#include "Material.h"
class PBR_Material : public Material
{
public:
	PBR_Material(float roughness, const Elite::RGBColor& albedo, bool metalness);

	Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const;
private:

	bool m_Metalness;
	float m_RoughnessSqr;
	Elite::RGBColor m_Albedo;
	
};

