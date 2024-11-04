#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include "PipelineTypes.h"
#include "Texture.h"

class IArchive;

typedef void(*TextureDataFreeFn)(void*);

struct UploadTextureFileOptions
{
	bool bRetainDataCPUMemory = true;
	int requiredComponents = 0; // 0 == auto, however many the file actually has. Other numbers can force more or less channels

	TextureFiltering MinifyFiltering = TextureFiltering::Nearest_MipMapNearest;
	TextureFiltering MagnifyFiltering = TextureFiltering::Nearest;
	TextureClamp TextureClampS = TextureClamp::Repeat;
	TextureClamp TextureClampT = TextureClamp::Repeat;
	glm::vec4 borderColour = glm::vec4{ 0,0,0,0 };
	bool bGenerateMipMaps = true;

};

class ENGINE_FRONTEND_API TextureReg
{
public:
	bool UploadTextureFile(const char* assetFolderPath, const char* textureName, const UploadTextureFileOptions* options=nullptr);
	bool RegisterTexture(const TextureData& data, TextureDataFreeFn freer);
	void Clear();
	void Serialize(IArchive& archive);
	void ReloadTextures(EMap<HTexture, HTexture>& outOldToNewHandleMap);
	HTexture GetTexture(const char* name) const;
private:
	EVec<EPair<TextureData, HTexture>> m_Textures;
	EMap<EString, TextureDataFreeFn> m_Freeers;
	EMap<EString, EPair<TextureData, HTexture>*> m_NameMap;
};