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
			ar->PushObj("ambientStrength");
			*ar << mc->ambientStrength;
			ar->PopObj();
			ar->PushObj("diffuseStrength");
			*ar << mc->diffuseStrength;
			ar->PopObj();
			ar->PushObj("speculatStrength");
			*ar << mc->speculatStrength;
			ar->PopObj();
			ar->PushObj("shininess");
			*ar << mc->shininess;
			ar->PopObj();
			ar->PushObj("hTexture");
			*ar << mc->hTexture;
			ar->PopObj();
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
			ar->PushObj("ambientStrength");
			*ar << mc->ambientStrength;
			ar->PopObj();
			ar->PushObj("diffuseStrength");
			*ar << mc->diffuseStrength;
			ar->PopObj();
			ar->PushObj("speculatStrength");
			*ar << mc->speculatStrength;
			ar->PopObj();
			ar->PushObj("shininess");
			*ar << mc->shininess;
			ar->PopObj();
			ar->PushObj("hTexture");
			*ar << mc->hTexture;
			ar->PopObj();
			break;
		}
		default:

			break;
		}

		ar->PopObj();
	}
}


META_IMPL(TestPipelineMaterial, MetaReg, SerializeC)