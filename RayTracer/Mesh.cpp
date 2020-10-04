#include "pch.h"
#include "Mesh.h"
#include "HitRecord.h"
#include "Material.h"
Mesh::Mesh(const Elite::FPoint3& origin, const Material& MAT)
	:SceneObject{origin,MAT}
	,m_Culling{Mesh::culling::non}
{
}

Mesh::Mesh(const Elite::FPoint3& origin, const Material& MAT, Mesh::culling culling)
	:SceneObject{origin,MAT}
	,m_Culling{culling}
{
}

bool Mesh::Hit(const Ray& ray, HitRecord& hitRecord) const
{
	HitRecord nearestHit{};
	nearestHit.intersection = FLT_MAX;
	for (unsigned int i{ 0 }; i < m_IndexBuffer.size(); i++)
	{
		switch (m_Culling)
		{
		case Mesh::culling::front_face:
			if (Elite::Dot(m_IndexBuffer[i].normal, ray.direction) < 0)
				continue;
			break;
		case Mesh::culling::back_face:
			if (Elite::Dot(m_IndexBuffer[i].normal, ray.direction) > 0)
				continue;
			break;
		default:
			if (Elite::Dot(ray.direction, m_IndexBuffer[i].normal) == 0)
				continue;
			break;
		}
		Elite::FVector3 l{};
		float t{};
		Elite::FPoint3 center{ (m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position.x + m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position.x + m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position.x) / 3 , (m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position.y + m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position.y + m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position.y) / 3 , (m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position.z + m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position.z + m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position.z) / 3 };
		l = center - ray.origin;
		t = Elite::Dot(l, m_IndexBuffer[i].normal) / Elite::Dot(ray.direction, m_IndexBuffer[i].normal);
		if (t < ray.tMin || t > ray.tMax)
			continue;

		Elite::FPoint3 intersectPlane{ ray.origin + t * ray.direction };

		Elite::FVector3 edge{ m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position - m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position };
		Elite::FVector3 PointToSide{ intersectPlane - m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position };
		if (Elite::Dot(m_IndexBuffer[i].normal, Elite::Cross(edge, PointToSide)) < 0)
			continue;
		edge = m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position - m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position;
		PointToSide = intersectPlane - m_VertexBuffer[m_IndexBuffer[i].idxVertex1].position ;
		if (Elite::Dot(m_IndexBuffer[i].normal, Elite::Cross(edge, PointToSide)) < 0)
			continue;
		edge = m_VertexBuffer[m_IndexBuffer[i].idxVertex0].position - m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position;
		PointToSide = intersectPlane - m_VertexBuffer[m_IndexBuffer[i].idxVertex2].position;
		if (Elite::Dot(m_IndexBuffer[i].normal, Elite::Cross(edge, PointToSide)) < 0)
			continue;

		hitRecord.intersection = t;
		hitRecord.hitPoint = ray.origin + hitRecord.intersection * ray.direction;
		hitRecord.normal = m_IndexBuffer[i].normal;
		hitRecord.direction = ray.direction;
		
		if (nearestHit.intersection > hitRecord.intersection)
		{
			nearestHit = hitRecord;
		}
	}
	hitRecord = nearestHit;
	return (hitRecord.intersection > ray.tMin&& hitRecord.intersection < ray.tMax);
}

void Mesh::AddVertex(const Elite::FPoint3& pos)
{
	Elite::FPoint3 nonRelativePos{};
	nonRelativePos.x = m_Origin.x + pos.x;
	nonRelativePos.y = m_Origin.y + pos.y;
	nonRelativePos.z = m_Origin.z + pos.z;
	m_VertexBuffer.push_back(Vertex{ nonRelativePos});
}

void Mesh::AddTriangle(unsigned int v0, unsigned int v1, unsigned int v2)
{
	MeshTriangleInfo t{};
	t.idxVertex0 = v0;
	t.idxVertex1 = v1;
	t.idxVertex2 = v2;
	Elite::FVector3 a{ m_VertexBuffer[v1].position - m_VertexBuffer[v0].position };
	Elite::FVector3 b{ m_VertexBuffer[v2].position - m_VertexBuffer[v0].position };
	t.normal = -Elite::GetNormalized(Elite::Cross(b, a));
	m_IndexBuffer.push_back(t);
}

