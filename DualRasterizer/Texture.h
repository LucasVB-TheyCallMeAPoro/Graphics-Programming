#pragma once
#include <string>
#include "SDL_image.h"
#include "EMath.h"
#include "ERGBColor.h"
class Texture
{
public:
	Texture(const std::string& file, ID3D11Device* pDevice);
	~Texture();
	Elite::RGBColor Sample(const Elite::FVector2& uv) const;
	Elite::RGBColor SampleAlpha(const Elite::FVector2& uv, float& alpha) const;
	ID3D11ShaderResourceView* GetShaderResource() const;
private:
	ID3D11ShaderResourceView* m_pTextureResourceView;
	ID3D11Texture2D* m_pTexture;
	SDL_Surface* m_pSurface;
};

