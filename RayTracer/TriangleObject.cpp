#include "pch.h"
#include "TriangleObject.h"
#include "HitRecord.h"
#include "Material.h"
#include <cmath>
TriangleObject::TriangleObject(const Elite::FPoint3& origin,  const Material& MAT, const TriangleObject::faceCulling& culling)
	:SceneObject{origin,MAT}
	,m_V0{ }
	,m_V1{  }
	,m_V2{ }
	,m_Culling{ culling}
	,m_Angle{0.f}
	,m_RotSpeed{1.f}
	,m_Normal{}
{

}

void TriangleObject::SetVertices(const Elite::FPoint3& v0, const Elite::FPoint3& v1, const Elite::FPoint3& v2)
{
	m_V0 = Elite::FPoint3{ v0.x,  v0.y, v0.z };
	m_V1 = Elite::FPoint3{v1.x,v1.y, v1.z };
	m_V2 = Elite::FPoint3{ v2.x, v2.y, v2.z };
	Elite::FVector3 a{ m_V1 - m_V0 };
	Elite::FVector3 b{ m_V2 - m_V0 };
	m_Normal = -Elite::GetNormalized(Elite::Cross(b, a));
}

bool TriangleObject::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	//Creating copy vertices & normal
	float cos = std::cos(m_Angle);
	float sin = std::sin(m_Angle);
	Elite::FMatrix4 m{
		cos, 0, sin, m_Origin.x,
		0,1,0, m_Origin.y,
		-sin , 0, cos, m_Origin.z,
		0,0,0,1
	};
	Elite::FPoint3 v0, v1, v2;
	v0 = (Elite::FPoint3)((Elite::FPoint4)(m * (Elite::FPoint4)m_V0));
	v1 = (Elite::FPoint3)((Elite::FPoint4)(m * (Elite::FPoint4)m_V1));
	v2 = (Elite::FPoint3)((Elite::FPoint4)(m * (Elite::FPoint4)m_V2));
	Elite::FVector3 normal;
	normal = (Elite::FVector3)(m * (Elite::FVector4)m_Normal);

	//actual hit function
	if (Elite::Dot(ray.direction, normal) == 0)
		return false;
	if (m_Culling == TriangleObject::faceCulling::back_face && (ray.inverse ? (Elite::Dot(normal, ray.direction) < 0) : (Elite::Dot(normal, ray.direction) > 0)))
		return false;
	if (m_Culling == TriangleObject::faceCulling::front_face && (ray.inverse ? (Elite::Dot(normal, ray.direction) > 0) : (Elite::Dot(normal, ray.direction) < 0)))
		return false;

	Elite::FVector3 l{};
	float t{};
	Elite::FPoint3 center{ (v0.x + v1.x + v2.x) / 3 , (v0.y +v1.y + v2.y) / 3 , (v0.z + v1.z + v2.z) / 3 };
	l = center - ray.origin;
	t = Elite::Dot(l, normal) / Elite::Dot(ray.direction, normal);
	if (t < ray.tMin || t > ray.tMax)
		return false;

	Elite::FPoint3 intersectPlane{ ray.origin + t * ray.direction };

	Elite::FVector3 edge{ v1 - v0 };
	Elite::FVector3 PointToSide{ intersectPlane - v0 };
	if (Elite::Dot(normal, Elite::Cross(edge, PointToSide)) < 0)
		return false;
	edge = v2 -v1;
	PointToSide = intersectPlane -v1;
	if (Elite::Dot(normal, Elite::Cross(edge, PointToSide)) < 0)
		return false;
	edge =v0 - v2;
	PointToSide = intersectPlane -v2;
	if (Elite::Dot(normal, Elite::Cross(edge, PointToSide)) < 0)
		return false;

	hitRecord.intersection = Elite::Dot((m_Origin - ray.origin), normal) / Elite::Dot(ray.direction, normal);
	hitRecord.hitPoint = ray.origin + hitRecord.intersection * ray.direction;
	hitRecord.normal = normal;
	hitRecord.direction = ray.direction;
	return (hitRecord.intersection > ray.tMin&& hitRecord.intersection < ray.tMax);
}

void TriangleObject::RotateTriangle(float elapsedSec)
{
	m_Angle += m_RotSpeed * elapsedSec;
}

