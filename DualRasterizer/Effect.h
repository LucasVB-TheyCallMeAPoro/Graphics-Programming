#pragma once
#include "pch.h"
class Effect
{
public:
	enum class ShaderType
	{
		phongShading,
		Transparency
	};
	Effect(ID3D11Device* pDevice, const std::wstring& path, const Effect::ShaderType type);
	~Effect();
	ID3DX11EffectTechnique* GetTechniquePoint() const;
	ID3DX11EffectTechnique* GetTechniqueLinear() const;
	ID3DX11EffectTechnique* GetTechniqueAnisotropic() const;
	ID3DX11Effect* GetEffect() const;
	Effect::ShaderType GetShaderType() const;
	void SetMatrices(Elite::FMatrix4& mWorldViewProj, Elite::FMatrix4& mWorld) const;
	void SetVector(Elite::FVector3& v) const;
	void SetDiffuseMap(ID3D11ShaderResourceView& ResourceView) const;
	void SetNormalMap(ID3D11ShaderResourceView& ResourceView) const;
	void SetGlossinessMap(ID3D11ShaderResourceView& ResourceView) const;
	void SetSpecualMap(ID3D11ShaderResourceView& ResourceView) const;
private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechniquePointSampling;
	ID3DX11EffectTechnique* m_pTechniqueLinearSampling;
	ID3DX11EffectTechnique* m_pTechniqueAnisotropicSampling;
	ID3DX11EffectMatrixVariable* m_pWorldViewProjectionMatrix;
	ID3DX11EffectMatrixVariable* m_pWorldMatrix;
	ID3DX11EffectVectorVariable* m_pCameraPostion;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;

	ShaderType m_ShaderType;
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& file);
	void LoadTechniques();
	void LoadMatrix();
	void LoadTexture();
};

