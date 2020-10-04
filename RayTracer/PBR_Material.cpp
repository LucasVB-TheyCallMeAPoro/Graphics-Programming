#include "pch.h"
#include "PBR_Material.h"
#include "PBR_BRDF.h"
#include "BRDF.h"
#include "HitRecord.h"
PBR_Material::PBR_Material(float roughness, const Elite::RGBColor& albedo, bool metalness)
	:m_RoughnessSqr{roughness * roughness}
	,m_Albedo{albedo}
	,m_Metalness{metalness}
{
}

Elite::RGBColor PBR_Material::Shade(const HitRecord& hit, const Elite::FVector3& incLight, const Elite::FVector3& outLight) const
{
	
	Elite::RGBColor f0{ (m_Metalness == false) ? Elite::RGBColor{0.04f,0.04f,0.04f}:m_Albedo };
	Elite::FVector3 halfVect{ Elite::GetNormalized(outLight + incLight) };
	Elite::RGBColor fresnel{ PBR_BRDF::Fresnel(halfVect,outLight,f0) };
	float normalDistribution{ PBR_BRDF::NormalDistribution(hit.normal,halfVect,m_RoughnessSqr) };
	float geometry{ PBR_BRDF::GeometryFunction(hit.normal,outLight,m_RoughnessSqr) * PBR_BRDF::GeometryFunction(hit.normal,incLight,m_RoughnessSqr) };
	Elite::RGBColor specular{ BRDF::Cook_Torrance(normalDistribution,fresnel,geometry,incLight,outLight,hit.normal) };

	Elite::RGBColor kd{ !m_Metalness ? Elite::RGBColor{1,1,1} - fresnel : Elite::RGBColor{0,0,0} };
	Elite::RGBColor diffuse{ BRDF::Lambert(kd,m_Albedo) };
	return diffuse + specular;
}
