#include "pch.h"
#include "Lambert_Material.h"
#include "BRDF.h"

Lambert_Material::Lambert_Material(float diffuseReflc, Elite::RGBColor color)
	:m_DiffuseReflectance{diffuseReflc}
	,m_DiffuseColor{color}
{
}

Elite::RGBColor Lambert_Material::Shade(const HitRecord& hit, const Elite::FVector3& w0, const Elite::FVector3& w1) const
{
	
	return BRDF::Lambert(m_DiffuseReflectance,m_DiffuseColor);
}
