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
				while (pMeta = pMeta->GetNext())
				{
					Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
					meta->Serialize(&archive, e, m_Reg);
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
			int num = archive.CountChildren();
			for (int i = 0; i < num; i++)
			{
				archive.PushChild(i);
				Entity e = m_Reg.create();
				Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
				while (pMeta = pMeta->GetNext())
				{
					Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
					meta->Serialize(&archive, e, m_Reg);
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
	auto v = m_Reg.view<TestPipelineMaterial>();
	// TODO URGENT: create a texture entity that the materials can use 
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
		if (pipelineHandleToMap.find(m.GetPipeline()) != pipelineHandleToMap.end())
		{
			if (oldToNewMeshMap.find(m.GetMesh()) != oldToNewMeshMap.end())
			{
				m.m_hMesh = oldToNewMeshMap.at(m.m_hMesh);
				if (PipeLine* pl = PipeLine::TryGetPipeline(pipelineHandleToMap.at(m.m_hPipeline).c_str()))
				{
					m.Create(m.GetMesh(), *pl, entity);
				}
				else
				{
					Err::ReportError(Err::FrontendErrorSeverity::Error, "Can't get pipeline '%s'", (u32) entity, pipelineHandleToMap.at(m.m_hPipeline).c_str());
				}
			}
			else
			{
				Err::ReportError(Err::FrontendErrorSeverity::Error, "Entity: '%i' using unknwon mesh: '%i'", (u32) entity, (u32)m.GetMesh());
			}
		}
		else
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "Entity: '%i' using unknwon pipeline: '%i'", (u32) entity, (u32)m.GetPipeline());
		}
	});
}