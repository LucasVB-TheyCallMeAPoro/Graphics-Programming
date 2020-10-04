#pragma once
#include "SceneObject.h"

class Plane : public SceneObject
{
public:
	Plane(const Elite::FPoint3& origin, const Elite::FVector3& normal, const Material& MAT);
	virtual bool Hit(const Ray& ray, HitRecord& hitrecord) const override;
private:
	Elite::FVector3 m_Normal;
};

