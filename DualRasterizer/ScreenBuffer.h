#pragma once
#include <stdlib.h>
struct SDL_Surface;
class ScreenBuffer
{
public:
	ScreenBuffer(SDL_Surface* screenBuffer);

	void SetPixel(size_t r, size_t c, const Elite::RGBColor& color);
	void SetPixel(size_t row, size_t col, const Elite::RGBColor& color, float alpha);
private:

	SDL_Surface* m_pScreenBuffer;
};