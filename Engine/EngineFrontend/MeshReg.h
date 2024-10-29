#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include "PipelineTypes.h"

class PipelineMeshData;
class IArchive;

class ENGINE_FRONTEND_API MeshReg
{
public:
	bool UploadMeshData(PipelineMeshData& data);
	bool UploadMeshData(const char* filePath);
	void Clear();
	PipelineMeshData* TryGetPipelineMeshData(HMesh mesh);
	void Serialize(IArchive& archive);
	void ReloadMeshes(EMap<HMesh, HMesh>& outOldToNewHandleMap);
	HMesh TryGetMesh(const EString& name) const;
private:
	EVec<EPair<PipelineMeshData, HMesh>> m_Meshes;
	EMap<EString, HMesh> m_NameToHandle;
	EMap<HMesh, PipelineMeshData*> m_HandleToData;
};
