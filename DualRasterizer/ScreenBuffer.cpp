#include "pch.h"
#include "ScreenBuffer.h"
#include "EMathUtilities.h"
ScreenBuffer::ScreenBuffer(SDL_Surface* screenBuffer)
	:m_pScreenBuffer{ screenBuffer }
{
}

void ScreenBuffer::SetPixel(size_t r, size_t c, const Elite::RGBColor& color)
{
	((uint32_t*)m_pScreenBuffer->pixels)[c + (r * m_pScreenBuffer->w)] = SDL_MapRGB(m_pScreenBuffer->format, static_cast<Uint8>(color.r * 255.f), static_cast<Uint8>(color.g * 255.f), static_cast<Uint8>(color.b * 255.f));
}

void ScreenBuffer::SetPixel(size_t row, size_t col, const Elite::RGBColor& color, float alpha)
{
	Uint8 r{};
	Uint8 g{};
	Uint8 b{};
	SDL_GetRGB(static_cast<Uint32*>(m_pScreenBuffer->pixels)[col + (row* m_pScreenBuffer->w)], m_pScreenBuffer->format, &r, &g, &b);
	Elite::RGBColor backgroundColor{ r / 255.f, g / 255.f, b / 255.f };
	SetPixel(row, col, Elite::Lerp(backgroundColor, color, alpha));
}
