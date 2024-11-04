#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>
class IArchive;

// KEEP THESE ENUMS IN SYNC WITH THE ONES IN EditorCommon PROJECT IN "UploadTextureFileMsg.h"

enum class TexFormat : u32
{
	R8G8B8,
	R8G8B8A8
};

enum class TextureRowAlign : u32
{
	Byte = 1,
	EvenByte = 2,
	Word = 4,
	DoubleWord = 8
};

enum class TextureFiltering : u32
{
	Nearest,
	Linear,
	Nearest_MipMapNearest,
	Linear_MipMapNearest,
	Nearest_MipMapLinear,
	Linear_MipMapLinear,
	NumOptions
};

enum class TextureClamp : u32
{
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder,
	NumOptions
};

struct TextureData
{
	EString Path;
	EString Name;
	size_t WidthPx = 0;
	size_t HeightPx = 0;
	TexFormat Format = TexFormat::R8G8B8;
	TextureRowAlign RowAlignment = TextureRowAlign::Byte;
	TextureFiltering MinifyFiltering = TextureFiltering::Nearest_MipMapNearest;
	TextureFiltering MagnifyFiltering = TextureFiltering::Nearest;
	TextureClamp TextureClampS = TextureClamp::Repeat;
	TextureClamp TextureClampT = TextureClamp::Repeat;
	glm::vec4 borderColour;
	bool bGenerateMipMaps = true;
	size_t DataSize = 0;
	uint8_t* pData = nullptr;
	void Serialize(IArchive* ar);
};

