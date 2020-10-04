#pragma once
#include "Material.h"
#include "ERGBColor.h"
class PhongLambert_Material : public Material
{	
public:
	PhongLambert_Material(float difRefl, float specRefl, const Elite::RGBColor& color, float exponent);
	virtual Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const;
private:
	float m_DiffuseReflectance;
	float m_SpecularReflectance;
	float m_PhongExponent;
	Elite::RGBColor m_DiffuseColor;
	
};

