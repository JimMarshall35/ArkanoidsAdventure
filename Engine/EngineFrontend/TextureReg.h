#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include "PipelineTypes.h"

struct TextureData;
class IArchive;

typedef void(*TextureDataFreeFn)(void*);

class ENGINE_FRONTEND_API TextureReg
{
public:
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