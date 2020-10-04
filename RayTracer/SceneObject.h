#pragma once
#include "Ray.h"
#include "EMath.h"
class Material;
struct HitRecord;
class SceneObject
{
public:
	SceneObject(const Elite::FPoint3& origin, const Material& MAT);
	~SceneObject() = default;
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const = 0;
	virtual const Material* GetMaterial() const;
protected:
	const Material* m_Material;
	const Elite::FPoint3 m_Origin;
};

