#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Effect.h"
#include <vector>
#include "Texture.h"
struct Vertex_Input
{
	Elite::FPoint3 position;
	Elite::FVector2 UV;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
};
struct Vertex_Output
{
	Elite::FPoint4 position;
	Elite::FVector2 UV;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
};

class Mesh
{
public:

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};
	enum class ShaderType
	{
		phong,
		transparent
	};
	enum class DrawTechnique
	{
		point = 0,
		linear = 1,
		anisotropic = 2

	};
	Mesh(ID3D11Device* pDevice, const Effect* pEffect, const std::vector<Vertex_Input>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, ShaderType shaderType);
	~Mesh();
	void Render(ID3D11DeviceContext* pDeviceContext, Elite::FMatrix4& mWorldViewProjection, Elite::FMatrix4& mWorld, Elite::FVector3& cameraPos, Texture* diffuseMap, Texture* normalMap, Texture* glossinessMap, Texture* specularMap) const;
	const Effect* GetEffect() const;
	PrimitiveTopology GetTopology() const;
	void ToggleSampling();

	ShaderType GetShaderType() const;
	const std::vector<Vertex_Input>& GetVertices() const;
	const std::vector<unsigned int>& GetIndices() const;
private:
	const Effect* m_pEffect;
	int m_AmountIndices;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11InputLayout* m_pVertexLayout;
	PrimitiveTopology m_Topology;
	std::vector<Vertex_Input> m_VertexBuffer;
	std::vector<unsigned int> m_IndexBuffer;
	DrawTechnique m_DrawTechnique;
	bool m_PrevKeyStateSampleToggle;
	ShaderType m_ShaderType;
};

