#pragma once
#include <vector>

#include "SceneObject.h"
#include "Triangle.h"
class Mesh : public SceneObject
{
public:
	enum class culling
	{
		front_face,
		back_face,
		non
	};
	Mesh(const Elite::FPoint3& origin,const Material& MAT);
	Mesh(const Elite::FPoint3& origin,const Material& MAT, Mesh::culling culling);
	virtual bool Hit(const Ray& ray, HitRecord& hitRecord) const override;
	virtual void AddVertex(const Elite::FPoint3& pos);
	virtual void AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2);
private:

	std::vector<Vertex> m_VertexBuffer;
	std::vector<MeshTriangleInfo> m_IndexBuffer;

	culling m_Culling;
};

