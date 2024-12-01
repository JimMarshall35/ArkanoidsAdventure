#pragma once
#include "EntityRegWrapper.h"
#include "IArchive.h"
#include "ComponentReg.h"
#include "TestPipeline.h"
#include "MeshComponent.h"
#include "IBackendApp.h"
#include "EngineDLLInterface.h"
#include "FrontendError.h"
void EntityRegWrapper::Clear()
{
	auto mc = m_Reg.view<MeshComponent>();
	mc.each([](MeshComponent& mc) 
	{
		Engine::GetAPI().DestroyDrawable(mc.GetDrawable());
	});
	m_Reg.clear();
}

void EntityRegWrapper::Serialize(IArchive& archive)
{
	const int version = 1;
	if (archive.IsStoring())
	{
		archive.PushObj("Entities");
		archive << version;
		auto v = m_Reg.view<entt::entity>();
		for (Entity e : v)
		{
			archive.PushObj("Entity");
				archive << (u32)e;
				Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
				while (pMeta)
				{
					Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
					meta->Serialize(&archive, e, m_Reg);
					pMeta = pMeta->GetNext();
				}
			archive.PopObj();
		}
		archive.PopObj();
	}
	else
	{
		archive.PushObj("Entities");
		int version;
		archive >> version;
		switch (version)
		{
		case 1:
		{
			
			std::unordered_map<u32, u32> oldToNewEntityMap;
			int num = archive.CountChildren();
			for (int i = 0; i < num; i++)
			{
				Entity e = m_Reg.create();
			}
			for (int i = 0; i < num; i++)
			{
				archive.PushChild(i);
				u32 entID = 0;
				archive >> entID;
				Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
				while (pMeta)
				{
					Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
					meta->Serialize(&archive, (Entity)entID, m_Reg);
					pMeta = pMeta->GetNext();
				}
				archive.PopObj();
			}
			break;

		}

		default:
			// @ERROR
			break;
		}
		archive.PopObj();
	}
}

void EntityRegWrapper::PostSerializeFix(const EMap<HPipeline, EString>& pipelineHandleToMap, const EMap<HTexture, HTexture>& oldToNewTextureMap, const EMap<HMesh, HMesh>& oldToNewMeshMap)
{
	

	Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
	while (pMeta)
	{
		((Comp::ComponentMeta*)pMeta)->OnSceneLoad(pipelineHandleToMap, oldToNewTextureMap, oldToNewMeshMap);
		pMeta = pMeta->GetNext();
	}

	/*
	auto v = m_Reg.view<TestPipelineMaterial>();
	v.each([&](TestPipelineMaterial& t)
	{
		if (oldToNewTextureMap.find(t.hTexture) != oldToNewTextureMap.end())
		{
			t.hTexture = oldToNewTextureMap.at(t.hTexture);
		}
	});
	auto v2 = m_Reg.view<MeshComponent>();
	v2.each([&pipelineHandleToMap, &oldToNewMeshMap](const auto entity, MeshComponent& m)
	{
		
	});*/
}