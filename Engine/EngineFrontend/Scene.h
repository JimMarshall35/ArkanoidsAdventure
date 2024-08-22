#pragma once
#include "EngineLib.h"
#include "PipelineTypes.h"
#include <entt/entt.hpp>
#include "PipelineMeshData.h"
#include "Texture.h"
#include "ExportMacro.h"

#include "Pipeline.h"
class IArchive;

namespace Scn
{

	class ENGINE_FRONTEND_API EntityRegWrapper
	{
	public:
		EntityReg& GetReg() { return m_Reg; }
		void Clear();
		void Serialize(IArchive& archive);
	private:
		EntityReg m_Reg;
	};

	class ENGINE_FRONTEND_API MeshReg
	{
	public:
		bool UploadMeshData(PipelineMeshData& data);
		void Clear();
		PipelineMeshData* TryGetPipelineMeshData(HMesh mesh);
		void Serialize(IArchive& archive);
	private:
		EVec<EPair<PipelineMeshData, HMesh>> m_Meshes;
		EMap<EString, HMesh> m_NameToHandle;
		EMap<HMesh, PipelineMeshData*> m_HandleToData;
	};

	typedef void(*TextureDataFreeFn)(void*);

	class ENGINE_FRONTEND_API TextureReg
	{
	public:
		bool RegisterTexture(const TextureData& data, TextureDataFreeFn freer);
		void Clear();
		void Serialize(IArchive& archive);
	private:
		EVec<EPair<TextureData, HTexture>> m_Textures;
		EMap<EString, TextureDataFreeFn> m_Freeers;
		EMap<EString, EPair<TextureData, HTexture>*> m_NameMap;
	};
	
	class ENGINE_FRONTEND_API PipelineReg
	{
	public:
		bool UploadPipelineData(PipeLine& piprlinr);
		void Clear();
		void Serialize(IArchive& archive);
	private:
		EVec<EPair<PipeLine, HPipeline>> m_Pipelines;
		EMap<EString, HPipeline> m_NameToHandle;
		EVec<u8> m_PipelineUserData;
		EVec<size_t> m_StartIndices;

	};

	struct ENGINE_FRONTEND_API Scene
	{
		PipelineReg pipelines;
		MeshReg meshReg;
		EntityRegWrapper entities;
		Entity rootTransformEntity;
		Entity activeCameraAntity;
	};

	bool IsSceneLoaded();
	Scene& GetScene();

	void SerializeScene(IArchive& archive);
}