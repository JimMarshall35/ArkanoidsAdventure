#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include "PipelineTypes.h"

struct TextureData;
class IArchive;

typedef void(*TextureDataFreeFn)(void*);

struct UploadTextureFileOptions
{
	bool bRetainDataCPUMemory = true;
	int requiredComponents = 0;
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