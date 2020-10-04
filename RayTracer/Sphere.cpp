#include "pch.h"
#include "Sphere.h"
#include "HitRecord.h"
#include "Material.h"
Sphere::Sphere(const Elite::FPoint3& origin, float radius, const Material& MAT)
	:SceneObject(origin, MAT)
	, m_Radius{ radius }
{
}

bool Sphere::Hit(const Ray& ray, HitRecord& hitrecord) const
{
	Elite::FVector3 l{ m_Origin - ray.origin };
	float tca{Elite::Dot(l,ray.direction)};

	Elite::FVector3 reject{ Elite::Reject(l,ray.direction) };
	float od{Elite::Magnitude(reject)};
	float od2{ od * od };
	float rad2{ m_Radius * m_Radius };
	if (od2 < rad2)
	{
		float thc{ sqrtf(rad2 - od2) };

		hitrecord.intersection = tca - thc;
		hitrecord.hitPoint = ray.origin + hitrecord.intersection * ray.direction;
		hitrecord.normal = (hitrecord.hitPoint - m_Origin) / m_Radius;
		hitrecord.direction = ray.direction;
		return (hitrecord.intersection > ray.tMin && hitrecord.intersection < ray.tMax);
	}
	return false;

}
