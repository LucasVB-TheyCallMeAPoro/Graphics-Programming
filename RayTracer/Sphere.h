#pragma once
#include "SceneObject.h"
class Sphere : public SceneObject
{
public:
	Sphere(const Elite::FPoint3& origin, float radius, const Material& MAT);
	virtual bool Hit(const Ray& ray, HitRecord& hitrecord) const override;
	
private:
	float m_Radius;
};

