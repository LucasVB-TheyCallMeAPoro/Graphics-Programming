#include "pch.h"
#include "Effect.h"
#include <sstream>
Effect::Effect(ID3D11Device* pDevice, const std::wstring& path, const Effect::ShaderType type)
	:m_ShaderType{type}
{
	m_pEffect = LoadEffect(pDevice, path);
	LoadTechniques();
	LoadMatrix();
	LoadTexture();

}

Effect::~Effect()
{
	m_pTechniquePointSampling->Release();
	m_pTechniqueLinearSampling->Release();
	m_pTechniqueAnisotropicSampling->Release();
	m_pEffect->Release();
	m_pDiffuseMapVariable->Release();
	m_pWorldViewProjectionMatrix->Release();
	m_pWorldMatrix->Release();
	m_pCameraPostion->Release();
	if (m_pNormalMapVariable != nullptr)
		m_pNormalMapVariable->Release();
	if (m_pGlossinessMapVariable != nullptr)
		m_pGlossinessMapVariable->Release();
	if (m_pSpecularMapVariable != nullptr)
		m_pSpecularMapVariable->Release();
}

ID3DX11EffectTechnique* Effect::GetTechniquePoint() const
{
	return m_pTechniquePointSampling;
}

ID3DX11EffectTechnique* Effect::GetTechniqueLinear() const
{
	return m_pTechniqueLinearSampling;
}

ID3DX11EffectTechnique* Effect::GetTechniqueAnisotropic() const
{
	return m_pTechniqueAnisotropicSampling;
}

ID3DX11Effect* Effect::GetEffect() const
{
	return m_pEffect;
}

Effect::ShaderType Effect::GetShaderType() const
{
	return m_ShaderType;
}

void Effect::SetMatrices(Elite::FMatrix4& mWorldViewProj, Elite::FMatrix4& mWorld) const
{
	m_pWorldViewProjectionMatrix->SetMatrix(&(mWorldViewProj[0][0]));
	m_pWorldMatrix->SetMatrix(&(mWorld[0][0]));
}

void Effect::SetVector(Elite::FVector3& v) const
{
	m_pCameraPostion->SetFloatVector(&(v.x,v.y,v.z));
}

void Effect::SetDiffuseMap(ID3D11ShaderResourceView& ResourceView) const
{
	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(&ResourceView);
}

void Effect::SetNormalMap(ID3D11ShaderResourceView& ResourceView) const
{
	switch (m_ShaderType)
	{
	case Effect::ShaderType::phongShading:
		if (m_pNormalMapVariable->IsValid())
			m_pNormalMapVariable->SetResource(&ResourceView);
		break;
	case Effect::ShaderType::Transparency:
		return;
		break;
	default:
		break;
	}
	
}

void Effect::SetGlossinessMap(ID3D11ShaderResourceView& ResourceView) const
{
	switch (m_ShaderType)
	{
	case Effect::ShaderType::phongShading:
		if (m_pGlossinessMapVariable->IsValid())
			m_pGlossinessMapVariable->SetResource(&ResourceView);
		break;
	case Effect::ShaderType::Transparency:
		return;
		break;
	default:
		break;
	}

}

void Effect::SetSpecualMap(ID3D11ShaderResourceView& ResourceView) const
{
	switch (m_ShaderType)
	{
	case Effect::ShaderType::phongShading:
		if (m_pSpecularMapVariable->IsValid())
			m_pSpecularMapVariable->SetResource(&ResourceView);
		break;
	case Effect::ShaderType::Transparency:
		return;
		break;
	default:
		break;
	}
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& file)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect{};

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(file.c_str(), nullptr, nullptr, shaderFlags,0,pDevice,&pEffect,&pErrorBlob);
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;
			
			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << file;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}
	return pEffect;
}

void Effect::LoadTechniques()
{
	m_pTechniquePointSampling = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechniquePointSampling->IsValid())
		std::cout << "TechniquePointSampling is not valid" << std::endl;
	m_pTechniqueLinearSampling = m_pEffect->GetTechniqueByName("TechniqueLinear");
	if (!m_pTechniqueLinearSampling->IsValid())
		std::cout << "TechniqueLinearSampling is not valid" << std::endl;
	m_pTechniqueAnisotropicSampling = m_pEffect->GetTechniqueByName("TechniqueAnisotropic");
	if (!m_pTechniqueAnisotropicSampling->IsValid())
		std::cout << "TechniqueAnisotropicSampling is not valid" << std::endl;
}

void Effect::LoadMatrix()
{
	m_pWorldViewProjectionMatrix = m_pEffect->GetVariableByName("gWorldViewProjectionMatrix")->AsMatrix();
	if (!m_pWorldViewProjectionMatrix->IsValid())
		std::cout << "m_pWorldViewProjectionMatrix not valid\n";
	m_pWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pWorldMatrix->IsValid())
		std::cout << "m_pWorldMatrix not valid\n";
	m_pCameraPostion = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
	if (!m_pCameraPostion->IsValid())
		std::cout << "m_pCameraPostion not valid\n";
}

void Effect::LoadTexture()
{
	switch (m_ShaderType)
	{
	case Effect::ShaderType::phongShading:
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::cout << "m_pDiffuseMapVariable not valid\n";
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
			std::cout << "m_pNormalMapVariable not valid\n";
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
			std::cout << "m_pGlossinessMapVariable not valid\n";
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
			std::cout << "m_pSpecularMapVariable not valid\n";
		break;
	case Effect::ShaderType::Transparency:
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
			std::cout << "m_pDiffuseMapVariable not valid\n";
		break;
	default:
		break;
	}

}
