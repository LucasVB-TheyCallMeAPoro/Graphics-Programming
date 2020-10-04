#pragma once
#include "Mesh.h"
#include "Effect.h"
#include <vector>
#include "Texture.h"
struct uInt3 
{
	unsigned int a;
	unsigned int b;
	unsigned int c;

	bool operator==(const uInt3& rhs) const
	{
		return a == rhs.a && b == rhs.b && c == rhs.c;
	}
};
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	void AddEffect(ID3D11Device* pDevice, const std::wstring& path, const Effect::ShaderType type);
	void AddMesh(ID3D11Device* pDevice, unsigned int idxEffect, std::vector<Vertex_Input>& vertexBuffer, std::vector<unsigned int>& indexBuffer, Mesh::ShaderType shaderType);
	void CreateMeshByFile(ID3D11Device* pDevice, unsigned int idxEffect, const std::string& path, Mesh::ShaderType shaderType);
	void AddDiffuseTexture(ID3D11Device* pDevice, const std::string& path);
	void AddNormalTexture(ID3D11Device* pDevice, const std::string& path);
	void AddGlossinessTexture(ID3D11Device* pDevice, const std::string& path);
	void AddSpecularTexture(ID3D11Device* pDevice, const std::string& path);
	Mesh* GetMesh(unsigned int idx) const;
	Texture* GetDiffuseTexture(unsigned int idx) const;
	Texture* GetNormalTexture(unsigned int idx) const;
	Texture* GetGlossinessTexture(unsigned int idx) const;
	Texture* GetSpecularTexture(unsigned int idx) const;
private:
	std::vector<Effect*> m_pEffects;
	std::vector<Mesh*> m_pMeshes;
	std::vector<Texture*> m_pDiffuseTextures;
	std::vector<Texture*> m_pNormalTextures;
	std::vector<Texture*> m_pGlossinessTextures;
	std::vector<Texture*> m_pSpecularTextures;
};