#pragma once
#include <stdint.h>
#include <string>

namespace EditorServer
{
	// these enums need to mirror the ones in EngineFrontend/Textures.h

	enum class TexFormat : uint8_t
	{
		R8G8B8,
		R8G8B8A8
	};

	enum class TextureRowAlign : uint8_t
	{
		Byte = 1,
		EvenByte = 2,
		Word = 4,
		DoubleWord = 8
	};

	enum class TextureFiltering : uint8_t
	{
		Nearest,
		Linear,
		Nearest_MipMapNearest,
		Linear_MipMapNearest,
		Nearest_MipMapLinear,
		Linear_MipMapLinear,
		NumOptions
	};

	enum class TextureClamp : uint8_t
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		NumOptions
	};

	struct UploadTextureFileOptions
	{
		UploadTextureFileOptions()
		{
			bGenerateMipMaps = true;
			bRetainDataCPUMemory = true;
		}

		int requiredComponents = 0; // 0 == auto, however many the file actually has. Other numbers can force more or less channels

		TextureFiltering MinifyFiltering = TextureFiltering::Nearest_MipMapNearest;
		TextureFiltering MagnifyFiltering = TextureFiltering::Nearest;
		TextureClamp TextureClampS = TextureClamp::Repeat;
		TextureClamp TextureClampT = TextureClamp::Repeat;
		//glm::vec4 borderColour = glm::vec4{ 0,0,0,0 };
		bool bGenerateMipMaps : 1;
		bool bRetainDataCPUMemory : 1;
	};

	struct UploadTextureFileMsg
	{
		UploadTextureFileOptions options;
		std::string assetFolderRelativePath;
		std::string name;
	};

}