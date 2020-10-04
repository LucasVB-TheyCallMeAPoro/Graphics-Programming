#pragma once
#include "Material.h"
class Lambert_Material : public Material
{
public:
	Lambert_Material(float diffuseReflc, Elite::RGBColor color);
	virtual Elite::RGBColor Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const override;
private:
	float m_DiffuseReflectance;
	Elite::RGBColor m_DiffuseColor;
};

