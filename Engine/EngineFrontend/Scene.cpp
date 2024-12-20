#include "Scene.h"
#include <entt/entt.hpp>
#include <optional>
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "FrontendError.h"
#include "Pipeline.h"
#include "IBackendApp.h"
#include "IArchive.h"
#include "ComponentReg.h"
#include "stb_image.h"
#include "TestPipeline.h"
#include "MeshComponent.h"
namespace Scn
{
	static Scene gTheScene;
	static bool gbSceneLoaded = true;

	bool IsSceneLoaded()
	{
		return gbSceneLoaded;
	}

	Scene& GetScene()
	{
		return gTheScene;
	}

	void PopulateHandleToPipelineNameMap(EMap<HPipeline, EString>& outMap, IArchive& archive)
	{
		archive.PushObj("Pipelines");
			i32 arraySize = 0;
			archive >> arraySize;
			for (int i = 0; i < arraySize; i++)
			{
				archive.PushChild(i);
					archive.PushObj("Name");
						EString name = "";
						archive >> name;
					archive.PopObj();
					archive.PushObj("Handle");
						u64 handle = 0;
						archive >> handle;
					archive.PopObj();
				archive.PopObj();
				outMap[handle] = name;
			}
		archive.PopObj();
	}

	void DeserialiseSceneV1(IArchive& archive)
	{
		Scene& s = GetScene();
		archive.PushObj("rootTransformEntity");
			archive >> s.rootTransformEntity;
		archive.PopObj();
		archive.PushObj("activeCameraAntity");
			archive >> s.activeCameraAntity;
		archive.PopObj();
		In::Serialize(archive);
		s.entities.Serialize(archive);
		s.meshReg.Serialize(archive);
		s.textureReg.Serialize(archive);
		EMap<HPipeline, EString> pipelineHandleToMap;
		PopulateHandleToPipelineNameMap(pipelineHandleToMap, archive);
		EMap<HTexture, HTexture> oldToNewTextureMap;
		EMap<HMesh, HMesh> oldToNewMeshMap;
		s.textureReg.ReloadTextures(oldToNewTextureMap);
		s.meshReg.ReloadMeshes(oldToNewMeshMap);
		s.entities.PostSerializeFix(pipelineHandleToMap, oldToNewTextureMap, oldToNewMeshMap);
   		Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
		while (pMeta)
		{
			Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
			meta->RegisterListeners(s.entities.GetReg());
			pMeta = pMeta->GetNext();
		}

		gbSceneLoaded = true;
	}

	EMap<HPipeline, EString> SerializePipelineHandles_Loading(IArchive& archive)
	{
		EMap<HPipeline, EString> rVal;
		int numPipelines = 0;
		archive.PushObj("Pipelines");
			archive >> numPipelines;
			for (int i = 0; i < numPipelines; i++)
			{
				archive.PushChild(i);
					HPipeline hPL;
					EString name;
					archive.PushObj("Handle");
						archive >> hPL;
					archive.PopObj();
					archive.PushObj("Name");
						archive >> name;
					archive.PopObj();
					EAssert(rVal.find(hPL) == rVal.end());
					rVal[hPL] = name;
				archive.PopObj();
			}
		archive.PopObj();
		return rVal;
	}

	void SerializePipelineHandles_Storing(IArchive& archive)
	{
		auto itr = [](PipeLine* pl, void* pUserData) -> bool
		{
			IArchive* ar = (IArchive*)pUserData;
			ar->PushObj("Pipeline");
				ar->PushObj("Handle");
					*ar << pl->GetH();
				ar->PopObj();
				ar->PushObj("Name");
					EString name = pl->GetName();
					*ar << name;
				ar->PopObj();
			ar->PopObj();
			return false;
		};
		archive.PushObj("Pipelines");
			archive << PipeLine::NumPipelines();
			PipeLine::IteratePipelines(itr, &archive);
		archive.PopObj();
	}

	void SerializeComponentTypes(IArchive* ar)
	{
		const EMap<EString, Comp::ComponentMeta*>&  map = Comp::GetComponentMetaMap();
		ar->PushObj("ComponentTypes");
		for (const std::pair<EString, Comp::ComponentMeta*>& p : map)
		{
			p.second->SerializeDefaultComponent(ar);
		}
		ar->PopObj();
	}

	void SerializeScene(IArchive& archive)
	{
		const u32 version = 1;
		if (archive.IsStoring())
		{
			archive << version;
			Scene& s = GetScene();
			archive.PushObj("rootTransformEntity");
				archive << s.rootTransformEntity;
			archive.PopObj();
			archive.PushObj("activeCameraAntity");
				archive << s.activeCameraAntity;
			archive.PopObj();

			In::Serialize(archive);
			s.entities.Serialize(archive);
			s.meshReg.Serialize(archive);
			s.textureReg.Serialize(archive);
			SerializePipelineHandles_Storing(archive);
			SerializeComponentTypes(&archive);
		}
		else
		{
			u32 v; 
			archive >> v;
			switch (v)
			{
			case 1:
				DeserialiseSceneV1(archive);
				break;
			default:
				break;
			}
		}
		

	}
	void ForceSceneLoadedTrue()
	{
	}
}
