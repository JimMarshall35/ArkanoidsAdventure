#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include "PipelineTypes.h"

class IArchive;

class ENGINE_FRONTEND_API EntityRegWrapper
{
public:
	EntityReg& GetReg() { return m_Reg; }
	void Clear();
	void Serialize(IArchive& archive);
	void PostSerializeFix(const EMap<HPipeline, EString>& pipelineHandleToMap, const EMap<HTexture, HTexture>& oldToNewTextureMap, const EMap<HMesh, HMesh>& oldToNewMeshMap);
private:
	EntityReg m_Reg;
};