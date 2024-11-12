#include "MeshComponent.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "Scene.h"
#include "IArchive.h"
#include "FrontendError.h"
#include "Pipeline.h"

void MeshComponent::Create(HMesh mesh, PipeLine& pipeline, entt::entity e)
{
	m_hMesh = mesh;
	m_hDrawable = pipeline.GetDrawable(mesh, e);
	m_hPipeline = pipeline.GetH();
}

void MeshComponent::Destroy()
{
	if (m_hDrawable != ENGINE_NULL_HANDLE)
	{
		Engine::GetAPI().DestroyDrawable(m_hDrawable);
		m_hDrawable = ENGINE_NULL_HANDLE;
	}
}

Sphere MeshComponent::GetMeshBoundingSphere() const
{
	EAssert(Scn::IsSceneLoaded());
	Scn::Scene& s = Scn::GetScene();
	if (PipelineMeshData* pmd = s.meshReg.TryGetPipelineMeshData(m_hMesh))
	{
		return pmd->GetBoundingSphere();
	}
	EAssert(false);
	return { { 0,0,0 }, 0 };
}

static void MetaReg(Comp::ComponentMeta* m)
{
	//using namespace entt::literals;
	//entt::meta_factory<MeshComponent> factory = entt::meta<MeshComponent>().type("MeshComponent"_hs);
	//factory.ctor<HMesh, HPipeline, Entity>()
	//	.data<nullptr, &MeshComponent::GetMesh, entt::as_is_t>("mesh"_hs)
	//	.data<nullptr, &MeshComponent::GetDrawable, entt::as_is_t>("drawable"_hs)
	//	.data<nullptr, &MeshComponent::GetPipeline, entt::as_is_t>("pipeline"_hs);
}

void MeshComponent::SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	const i32 version = 1;
	{
		if (ar->IsStoring())
		{
			if (MeshComponent* mc = reg.try_get<MeshComponent>(e))
			{
				ar->PushObj("MeshComponent");
					*ar << version;
					ar->PushObj("HMesh");
						*ar << mc->GetMesh();
						ar->HintUsage(HandleType::Mesh);
					ar->PopObj();	
					ar->PushObj("HPipeline");
						*ar << mc->GetPipeline();
						ar->HintUsage(HandleType::Pipeline);
					ar->PopObj();
					ar->PushObj("HDrawable");
						*ar << mc->GetDrawable();
						ar->HintUsage(HandleType::Drawable);
					ar->PopObj();
				ar->PopObj();
			}
		}
		else
		{
			int fileVersion = 0;
			if (!ar->PushObj("MeshComponent")) return;
				MeshComponent* mc = reg.try_get<MeshComponent>(e);
				if (!mc)
				{
					reg.emplace<MeshComponent>(e);
					mc = reg.try_get<MeshComponent>(e);
				}
				*ar >> fileVersion;
				switch (fileVersion)
				{
				case 1:
				{
					ar->PushObj("HMesh");
						*ar >> mc->m_hMesh;
					ar->PopObj();	
					ar->PushObj("HPipeline");
						*ar >> mc->m_hPipeline;
					ar->PopObj();
					ar->PushObj("HDrawable");
						*ar >> mc->m_hDrawable;
					ar->PopObj();
				}
				default:

					break;
				}
				
			ar->PopObj();
		}
	}
}
static void OnMeshCmpntDestroy(entt::registry& r, entt::entity e)
{
	MeshComponent& mc = r.get<MeshComponent>(e);
	if (mc.m_hDrawable != ENGINE_NULL_HANDLE && mc.m_hMesh != ENGINE_NULL_HANDLE && mc.m_hPipeline != ENGINE_NULL_HANDLE)
	{
		mc.Destroy();
	}
};

static void OnMeshComponentCreate(entt::registry& r, entt::entity e)
{


}

static void OnMeshComponentUpdate(entt::registry& r, entt::entity e)
{

}

// using the normal META_IMPL macro here causes a null function ptr in the v table entry for EmplaceDefaultComponent?!
META_IMPL_EX(MeshComponent, MetaReg, MeshComponent::SerializeC, OnMeshComponentCreate, OnMeshCmpntDestroy, OnMeshComponentUpdate)