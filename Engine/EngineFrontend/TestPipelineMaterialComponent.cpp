#include "TestPipelineMaterialComponent.h"
#include "IArchive.h"
#include "Scene.h"

static void MetaReg(Comp::ComponentMeta* m) {}

static void SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	const i32 version = 1;

	if (ar->IsStoring())
	{
		if (TestPipelineMaterial* mc = reg.try_get<TestPipelineMaterial>(e))
		{
			ar->PushObj("TestPipelineMaterial");
				*ar << version;
				AR_STORE(mc, ambientStrength)
				AR_STORE(mc, diffuseStrength)
				AR_STORE(mc, specularStrength)
				AR_STORE(mc, shininess)
				AR_STORE_TEXTURE(mc, hTexture)
			ar->PopObj();
		}
	}
	else
	{
		int fileVersion = 0;
		if (!ar->PushObj("TestPipelineMaterial")) return;
		TestPipelineMaterial* mc = reg.try_get<TestPipelineMaterial>(e);
		if (!mc)
		{
			reg.emplace<TestPipelineMaterial>(e);
			mc = reg.try_get<TestPipelineMaterial>(e);
		}
		*ar >> fileVersion;
		switch (fileVersion)
		{
		case 1:
		{
			AR_LOAD(mc, ambientStrength)
			AR_LOAD(mc, diffuseStrength)
			AR_LOAD(mc, specularStrength)
			AR_LOAD(mc, shininess)
			AR_LOAD(mc, hTexture)
			break;
		}
		default:

			break;
		}

		ar->PopObj();
	}
}

static void OnSceneLoaded(const EMap<HPipeline, EString>& pipelineHandleToMap, const EMap<HTexture, HTexture>& oldToNewTextureMap, const EMap<HMesh, HMesh>& oldToNewMeshMap)
{
	Scn::Scene& s = Scn::GetScene();
	EntityReg& r = s.entities.GetReg();
	auto v = r.view<TestPipelineMaterial>();
	v.each([&](Entity e, TestPipelineMaterial& mc)
	{
		if (oldToNewMeshMap.find(mc.hTexture) != oldToNewTextureMap.end())
		{
			mc.hTexture = oldToNewMeshMap.at(mc.hTexture);
		}
		else
		{
			assert(false);
		}
	});
}

static void OnMeshCmpntDestroy(entt::registry& r, entt::entity e)
{

};

static void OnMeshComponentCreate(entt::registry& r, entt::entity e)
{
}

static void OnMeshComponentUpdate(entt::registry& r, entt::entity e)
{

}
META_IMPL(TestPipelineMaterial, MetaReg, SerializeC, OnMeshComponentCreate, OnMeshCmpntDestroy, OnMeshComponentUpdate, OnSceneLoaded)