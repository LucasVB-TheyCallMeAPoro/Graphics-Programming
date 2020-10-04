#include "pch.h"
#include "Texture.h"
Texture::Texture(const std::string& file, ID3D11Device* pDevice)
	:m_pTexture{nullptr}
	,m_pTextureResourceView{nullptr}
	,m_pSurface{nullptr}
{
	m_pSurface = IMG_Load(file.c_str());
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_pSurface->w;
	desc.Height = m_pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = m_pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

	HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pTexture);
	if (FAILED(hr))
		return;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pTextureResourceView);
	if (FAILED(hr))
		return;
	
}

Texture::~Texture()
{
	m_pTexture->Release();
	m_pTextureResourceView->Release();
	SDL_FreeSurface(m_pSurface);
}
Elite::RGBColor Texture::Sample(const Elite::FVector2& uv) const
{
	Elite::FVector2 remappedUV{ uv.x * m_pSurface->w,uv.y * m_pSurface->h };
	Uint32 pixel = static_cast<unsigned int>(remappedUV.y)* m_pSurface->w + static_cast<unsigned int>(remappedUV.x);
	Uint8 r{};
	Uint8 g{};
	Uint8 b{};
	SDL_GetRGB(static_cast<Uint32*>(m_pSurface->pixels)[pixel], m_pSurface->format, &r, &g, &b);
	return Elite::RGBColor{ r / 255.f, g / 255.f, b / 255.f };

}
Elite::RGBColor Texture::SampleAlpha(const Elite::FVector2& uv, float& alpha) const
{
	Elite::FVector2 ClampedUV{ Elite::Clamp(uv.x,0.f,1.f), Elite::Clamp(uv.y,0.f,1.f) };
	Elite::FVector2 remappedUV{ ClampedUV.x * m_pSurface->w,ClampedUV.y * m_pSurface->h };
	Uint32 pixel = static_cast<unsigned int>(remappedUV.y)* m_pSurface->w + static_cast<unsigned int>(remappedUV.x);
	Uint8 r{};
	Uint8 g{};
	Uint8 b{};
	Uint8 a{};
	SDL_GetRGBA(static_cast<Uint32*>(m_pSurface->pixels)[pixel], m_pSurface->format, &r, &g, &b, &a);
	alpha = a / 255.f;
	return Elite::RGBColor{ r / 255.f, g / 255.f, b / 255.f };
}
ID3D11ShaderResourceView* Texture::GetShaderResource() const
{
	return m_pTextureResourceView;
}

