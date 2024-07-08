#include "pch.h"
#include "Rasterize.h"
#include <algorithm>

namespace Rasterizer
{

	void SetPixel(uint8_t* dest, int destWidth, int x, int y, const uint8_t* color)
	{
		uint8_t* pixel = &dest[x * 3 + y * (destWidth * 3)];
		pixel[0] = color[0];
		pixel[1] = color[1];
		pixel[2] = color[2];
	}


	/*
	https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling
	Copyright Dmitry V. Sokolov

	This software is provided 'as-is', without any express or implied warranty.
	In no event will the authors be held liable for any damages arising from the use of this software.
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it freely,
	subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
	*/
	void RasterizeTriangle(uint8_t* dest, int destWidth, const int* t0, const int* t1, const int* t2, const uint8_t* color)
	{
		if (t0[1] > t1[1]) std::swap(t0, t1);
		if (t0[1] > t2[1]) std::swap(t0, t2);
		if (t1[1] > t2[1]) std::swap(t1, t2);
		int total_height = t2[1] - t0[1];
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1[1] - t0[1] || t1[1] == t0[1];
			int segment_height = second_half ? t2[1] - t1[1] : t1[1] - t0[1];
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1[1] - t0[1] : 0)) / segment_height;
			int A[2], B[2];
			for (int j = 0; j < 2; j++) {
				A[j] = int(t0[j] + (t2[j] - t0[j]) * alpha);
				B[j] = int(second_half ? t1[j] + (t2[j] - t1[j]) * beta : t0[j] + (t1[j] - t0[j]) * beta);
			}
			if (A[0] > B[0]) std::swap(A, B);
			for (int j = A[0]; j <= B[0]; j++)
				SetPixel(dest, destWidth, j, t0[1] + i, color);
		}
	}

	// https://github.com/miloyip/line/blob/master/line_bresenham.c
	// License: public domain.
	void RasterizeLine(uint8_t* dest, int destWidth, const int* p1, const int* p2, const uint8_t* color)
	{
		const int dx = abs(p2[0] - p1[0]), sx = p1[0] < p2[0] ? 1 : -1;
		const int dy = abs(p2[1] - p1[1]), sy = p1[1] < p2[1] ? 1 : -1;
		int err = (dx > dy ? dx : -dy) / 2;
		int current[2];
		current[0] = p1[0];
		current[1] = p1[1];
		while (SetPixel(dest, destWidth, current[0], current[1], color), current[0] != p2[0] || current[1] != p2[1])
		{
			const int e2 = err;
			if (e2 > -dx) { err -= dy; current[0] += sx; }
			if (e2 < dy) { err += dx; current[1] += sy; }
		}
	}
}