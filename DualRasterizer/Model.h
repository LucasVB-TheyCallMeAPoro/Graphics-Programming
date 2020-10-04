#pragma once
#include "Mesh.h"
class PerspectiveCamera;

class Model
{
public:
	enum class TextureToSample
	{
		diffuse,
		normal,
		glossiness,
		specular
	};
	Model(Mesh* pMesh, Texture* pDiffuse, Texture* pNormal, Texture* pGlossiness, Texture* pSpecular,const Elite::FPoint3& origin);
	Model(Mesh* pMesh, Texture* pDiffuse, const Elite::FPoint3& origin);
	void Update();
	void Rotate(const Elite::FVector3& axis, float angle);
	void Render(ID3D11DeviceContext* pDeviceContext, const PerspectiveCamera& camera) const;
	Elite::RGBColor SampleTexture(const Elite::FVector2& uv, TextureToSample texToSamp) const;
	Elite::RGBColor SampleTextureWithTransparency(const Elite::FVector2& uv, float& alpha,TextureToSample texToSamp) const;
	Elite::FMatrix4 GetTransform() const;
	Mesh* GetMesh() const;
	float GetShininess() const;
private:
	Mesh* m_pMesh;
	Texture* m_pDiffuseMap;
	Texture* m_pNormalMap;
	Texture* m_pGlossinessMap;
	Texture* m_pSpecualMap;
	float m_Shininess;
	Elite::FVector3 m_Translate;
	Elite::FMatrix3 m_Rotation;
};