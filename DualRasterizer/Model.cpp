#include "pch.h"
#include "Model.h"
#include "PerspectiveCamera.h"
Model::Model(Mesh* pMesh, Texture* pDiffuse, Texture* pNormal, Texture* pGlossiness, Texture* pSpecular,const Elite::FPoint3& origin)
	:m_pMesh{pMesh}
	,m_Translate{origin}
	, m_Rotation{ Elite::FMatrix3::Identity() }
	,m_pDiffuseMap{pDiffuse}
	,m_pNormalMap{pNormal}
	,m_pGlossinessMap{pGlossiness}
	,m_pSpecualMap{pSpecular}
	,m_Shininess{25.f}
{
}
Model::Model(Mesh* pMesh, Texture* pDiffuse, const Elite::FPoint3& origin)
	:m_pMesh{pMesh}
	,m_pDiffuseMap{pDiffuse}
	,m_pNormalMap{nullptr}
	,m_pGlossinessMap{nullptr}
	,m_pSpecualMap{nullptr}
	,m_Translate{ origin }
	,m_Rotation{ Elite::FMatrix3::Identity() }
	, m_Shininess{ 25.f }
{
}

void Model::Update()
{
	m_pMesh->ToggleSampling();
}
void Model::Rotate(const Elite::FVector3& axis, float angle)
{
	m_Rotation = Elite::MakeRotation(-angle, axis) * m_Rotation;
}
void Model::Render(ID3D11DeviceContext* pDeviceContext, const PerspectiveCamera& camera) const
{
	Elite::FMatrix4 WorldMatrix{ GetTransform() };
	Elite::FMatrix4 WorldViewProjectionMatrix{camera.GetProjectionMatrix() * camera.GetViewMatrix() * WorldMatrix };
	Elite::FVector3 CameraPos{ camera.GetPosition() };
	
	m_pMesh->Render(pDeviceContext, WorldViewProjectionMatrix, WorldMatrix, CameraPos, m_pDiffuseMap, m_pNormalMap, m_pGlossinessMap, m_pSpecualMap);

}

Elite::RGBColor Model::SampleTexture(const Elite::FVector2& uv, TextureToSample texToSamp) const
{
	TextureToSample	textureToSample = texToSamp;
	switch (textureToSample)
	{
	case Model::TextureToSample::diffuse:
		return m_pDiffuseMap->Sample(uv);
		break;
	case Model::TextureToSample::normal:
		return m_pNormalMap->Sample(uv);
		break;
	case Model::TextureToSample::glossiness:
		return m_pGlossinessMap->Sample(uv);
		break;
	case Model::TextureToSample::specular:
		return m_pSpecualMap->Sample(uv);
		break;
	}
}

Elite::RGBColor Model::SampleTextureWithTransparency(const Elite::FVector2& uv, float& alpha, TextureToSample texToSamp) const
{
	TextureToSample	textureToSample = texToSamp;
	switch (textureToSample)
	{
	case Model::TextureToSample::diffuse:
		return m_pDiffuseMap->SampleAlpha(uv, alpha);
		break;
	case Model::TextureToSample::normal:
		return m_pNormalMap->SampleAlpha(uv, alpha);
		break;
	case Model::TextureToSample::glossiness:
		return m_pGlossinessMap->SampleAlpha(uv, alpha);
		break;
	case Model::TextureToSample::specular:
		return m_pSpecualMap->SampleAlpha(uv, alpha);
		break;
	}
}

Elite::FMatrix4 Model::GetTransform() const
{
	return Elite::FMatrix4{ m_Rotation, m_Translate };
}

Mesh* Model::GetMesh() const
{
	return m_pMesh;
}

float Model::GetShininess() const
{
	return m_Shininess;
}
