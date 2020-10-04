#pragma once
#include "SceneObject.h"
class TriangleObject : public SceneObject
{
public:
	enum class faceCulling
	{
		front_face,
		back_face,
		non
	};
	TriangleObject(const Elite::FPoint3& origin,const Material& MAT, const TriangleObject::faceCulling& culling = TriangleObject::faceCulling::non);

	virtual void SetVertices(const Elite::FPoint3& v0, const Elite::FPoint3& v1, const Elite::FPoint3& v2);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const;
	virtual void RotateTriangle(float elapsedSec);
private:
	Elite::FPoint3 m_V0;
	Elite::FPoint3 m_V1;
	Elite::FPoint3 m_V2;

	Elite::FVector3 m_Normal;
	faceCulling m_Culling;

	float m_Angle;
	float m_RotSpeed;
};

