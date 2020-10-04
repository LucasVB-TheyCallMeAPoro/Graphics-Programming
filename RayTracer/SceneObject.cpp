#include "pch.h"
#include "SceneObject.h"
#include "Material.h"
SceneObject::SceneObject(const Elite::FPoint3& origin, const Material& MAT)
	:m_Origin{origin}
	,m_Material{ &MAT }
{
}

const Material* SceneObject::GetMaterial() const
{
	return m_Material;
}
