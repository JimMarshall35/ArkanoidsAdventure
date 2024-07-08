#pragma once
#include "EngineLib.h"

enum class TexFormat
{
	R8G8B8,
	R8G8B8A8
};

enum class TextureRowAlign
{
	Byte = 1,
	EvenByte = 2,
	Word = 4,
	DoubleWord = 8
};
struct TextureData
{
	size_t WidthPx = 0;
	size_t HeightPx = 0;
	TexFormat Format = TexFormat::R8G8B8;
	TextureRowAlign RowAlignment = TextureRowAlign::Byte;
	size_t DataSize = 0;
	uint8_t* pData;
};

