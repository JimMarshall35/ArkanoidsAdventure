#include "TestPipelineMaterialComponent.h"
#include "IArchive.h"

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
				AR_STORE(mc, hTexture)
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


META_IMPL(TestPipelineMaterial, MetaReg, SerializeC)