#include "pch.h"
#include "Plane.h"
#include "HitRecord.h"
#include "Material.h"
Plane::Plane(const Elite::FPoint3& origin, const Elite::FVector3& normal, const Material& MAT)
	: SceneObject{ origin,MAT }
	,m_Normal{normal}
{
}

bool Plane::Hit(const Ray& ray, HitRecord& hitrecord) const
{
	hitrecord.intersection = Elite::Dot((m_Origin - ray.origin), m_Normal) / Elite::Dot(ray.direction, m_Normal);
	hitrecord.hitPoint = ray.origin + hitrecord.intersection * ray.direction;
	hitrecord.normal = m_Normal;
	hitrecord.direction = ray.direction;
	return (hitrecord.intersection > ray.tMin && hitrecord.intersection < ray.tMax);
}
