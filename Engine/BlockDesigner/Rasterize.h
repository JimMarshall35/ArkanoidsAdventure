#pragma once
#include <stdint.h>

namespace Rasterizer
{
	void SetPixel(uint8_t* dest, int destWidth, int x, int y, const uint8_t* color);
	void RasterizeTriangle(uint8_t* dest, int destWidth, const int* t0, const int* t1, const int* t2, const uint8_t* color);
	void RasterizeLine(uint8_t* dest, int destWidth, const int* p1, const int* p2, const uint8_t* color);
}