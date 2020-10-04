#include "pch.h"
#include "PhongLambert_Material.h"
#include "BRDF.h"
#include "HitRecord.h"
PhongLambert_Material::PhongLambert_Material(float difRefl, float specRefl, const Elite::RGBColor& color, float exponent)
	:m_DiffuseReflectance{difRefl}
	,m_SpecularReflectance{specRefl}
	,m_DiffuseColor{color}
	,m_PhongExponent{exponent}
{
}

Elite::RGBColor PhongLambert_Material::Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const
{
	return BRDF::Lambert(m_DiffuseReflectance,m_DiffuseColor) + BRDF::Phong(m_SpecularReflectance,m_PhongExponent,incLight,outLight,hit.normal);
}
