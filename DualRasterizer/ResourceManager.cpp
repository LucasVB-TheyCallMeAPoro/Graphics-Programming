#include "pch.h"
#include "ResourceManager.h"
#include <fstream>
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (int i{ 0 }; i < m_pEffects.size(); ++i)
		delete m_pEffects[i];
	for (int i{ 0 }; i < m_pDiffuseTextures.size(); ++i)
		delete m_pDiffuseTextures[i];
	for (int i{ 0 }; i < m_pNormalTextures.size(); ++i)
		delete m_pNormalTextures[i];
	for (int i{ 0 }; i < m_pGlossinessTextures.size(); ++i)
		delete m_pGlossinessTextures[i];
	for (int i{ 0 }; i < m_pSpecularTextures.size(); ++i)
		delete m_pSpecularTextures[i];
	for (int i{ 0 }; i < m_pMeshes.size(); ++i)
		delete m_pMeshes[i];
}

void ResourceManager::AddEffect(ID3D11Device* pDevice, const std::wstring& path, const Effect::ShaderType type)
{
	m_pEffects.push_back(new Effect{ pDevice,path, type});
}

void ResourceManager::AddMesh(ID3D11Device* pDevice, unsigned int idxEffect, std::vector<Vertex_Input>& vertexBuffer,  std::vector<unsigned int>& indexBuffer, Mesh::ShaderType shaderType)
{
	
	m_pMeshes.push_back(new Mesh{ pDevice,m_pEffects[idxEffect], std::ref(vertexBuffer),std::ref(indexBuffer),shaderType });
}

void ResourceManager::CreateMeshByFile(ID3D11Device* pDevice, unsigned int idxEffect, const std::string& path, Mesh::ShaderType shaderType)
{
	std::vector<std::string> coord;
	std::ifstream in(path);
	std::vector<uInt3> faceBuffer;
	std::vector<Elite::FVector2> uvBuffer;
	std::vector<Elite::FVector3> normalBuffer;
	std::vector<Elite::FPoint3> posBuffer;
	std::vector<Vertex_Input> vertexBuffer;
	std::vector<unsigned int> idexBuffer;
	if (!in.is_open())
	{
		std::cout << "can not open file " << path << std::endl;
	}
	char buf[256];
	while (!in.eof())
	{
		in.getline(buf, 256);
		coord.push_back(std::string{ buf });
	}
	for (int i{ 0 }; i < coord.size(); ++i)
	{
		if (coord[i][0] == '#' || coord[i][0] == 'o' || coord[i][0] == 'g')
			continue;
		else if (coord[i][0] == 'v' && coord[i][1] == ' ')
		{
			float tmpx, tmpy, tmpz;
			sscanf_s(coord[i].c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);
			posBuffer.push_back(Elite::FPoint3{ tmpx, tmpy, -tmpz });
		}
		else if (coord[i][0] == 'f')
		{
			unsigned int v0, v1, v2;
			unsigned int vt0, vt1, vt2;
			unsigned int vn0, vn1, vn2;
			sscanf_s(coord[i].c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &v0, &vt0, &vn0, &v1, &vt1, &vn1, &v2, &vt2, &vn2);
			faceBuffer.push_back(uInt3{ v0,vt0,vn0 });
			faceBuffer.push_back(uInt3{ v1,vt1,vn1 });
			faceBuffer.push_back(uInt3{ v2,vt2,vn2 });

		}
		else if (coord[i][0] == 'v' && coord[i][1] == 't')
		{
			float uvX, uvY, uvZ;
			sscanf_s(coord[i].c_str(), "vt %f %f %f", &uvX, &uvY, &uvZ);
			uvBuffer.push_back(Elite::FVector2{ uvX,1.f - uvY });
		}
		else if (coord[i][0] == 'v' && coord[i][1] == 'n')
		{
			float x, y, z;
			sscanf_s(coord[i].c_str(), "vn %f %f %f", &x, &y, &z);
			normalBuffer.push_back(Elite::FVector3{ x,y,z });
		}
	}

	for (int i{ 0 }; i < faceBuffer.size(); ++i)
	{
		bool dup{ false };
		for (int j{ 0 }; j < i; ++j)
		{
			if (posBuffer[faceBuffer[i].a - 1] == posBuffer[faceBuffer[j].a - 1] && uvBuffer[faceBuffer[i].b - 1] == uvBuffer[faceBuffer[j].b - 1] && normalBuffer[faceBuffer[i].c - 1] == normalBuffer[faceBuffer[j].c - 1])
			{
				dup = true;
				idexBuffer.push_back(idexBuffer[j]);
				break;
			}
		}
		if (dup)
			continue;
		vertexBuffer.push_back(Vertex_Input{ posBuffer[faceBuffer[i].a - 1],uvBuffer[faceBuffer[i].b - 1], normalBuffer[faceBuffer[i].c - 1] });
		idexBuffer.push_back(vertexBuffer.size() - 1);
	}
	for (size_t i{ 0 }; i < idexBuffer.size() - 2; i += 3)
	{
		size_t v0 = idexBuffer[i];
		size_t v1 = idexBuffer[i + 1];
		size_t v2 = idexBuffer[i + 2];

		const Elite::FPoint4& p0 = vertexBuffer[v0].position;
		const Elite::FPoint4& p1 = vertexBuffer[v1].position;
		const Elite::FPoint4& p2 = vertexBuffer[v2].position;
		const Elite::FVector2& uv0 = vertexBuffer[v0].UV;
		const Elite::FVector2& uv1 = vertexBuffer[v1].UV;
		const Elite::FVector2& uv2 = vertexBuffer[v2].UV;

		const Elite::FVector3 edge0 = static_cast<Elite::FPoint3>(p1) - static_cast<Elite::FPoint3>(p0);
		const Elite::FVector3 edge1 = static_cast<Elite::FPoint3>(p2) - static_cast<Elite::FPoint3>(p0);
		const Elite::FVector2 diffX = Elite::FVector2{ uv1.x - uv0.x, uv2.x - uv0.x };
		const Elite::FVector2 diffY = Elite::FVector2{ uv1.y - uv0.y, uv2.y - uv0.y };
		float r = 1.f / Elite::Cross(diffX, diffY);

		Elite::FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertexBuffer[v0].tangent += tangent;
		vertexBuffer[v1].tangent += tangent;
		vertexBuffer[v2].tangent += tangent;
	}

	for (auto& v : vertexBuffer)
	{
		v.tangent = Elite::GetNormalized(Elite::Reject(v.tangent, v.normal));
		v.normal.z = -v.normal.z;
		v.tangent = -v.tangent;
	}
	m_pMeshes.push_back( new Mesh{pDevice, m_pEffects[idxEffect], vertexBuffer, idexBuffer, shaderType} );
}

void ResourceManager::AddDiffuseTexture(ID3D11Device* pDevice, const std::string& path)
{
	m_pDiffuseTextures.push_back(new Texture{ path, pDevice });
}

void ResourceManager::AddNormalTexture(ID3D11Device* pDevice, const std::string& path)
{
	m_pNormalTextures.push_back(new Texture{ path, pDevice });
}

void ResourceManager::AddGlossinessTexture(ID3D11Device* pDevice, const std::string& path)
{
	m_pGlossinessTextures.push_back(new Texture{ path, pDevice });
}

void ResourceManager::AddSpecularTexture(ID3D11Device* pDevice, const std::string& path)
{
	m_pSpecularTextures.push_back(new Texture{ path, pDevice });
}

Mesh* ResourceManager::GetMesh(unsigned int idx) const
{
	return m_pMeshes[idx];
}

Texture* ResourceManager::GetDiffuseTexture(unsigned int idx) const
{
	return m_pDiffuseTextures[idx];
}

Texture* ResourceManager::GetNormalTexture(unsigned int idx) const
{
	return m_pNormalTextures[idx];
}

Texture* ResourceManager::GetGlossinessTexture(unsigned int idx) const
{
	return m_pGlossinessTextures[idx];
}

Texture* ResourceManager::GetSpecularTexture(unsigned int idx) const
{
	return m_pSpecularTextures[idx];
}
