#include "PointLightComponent.h"
#include "IArchive.h"
#include "Scene.h"

static void SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	const i32 version = 1;

	if (ar->IsStoring())
	{
		if (PointLightComponent* mc = reg.try_get<PointLightComponent>(e))
		{
			ar->PushObj("PointLightComponent");
				*ar << version;
				ar->PushObj("Colour");
					*ar << mc->colour;
				ar->PopObj();
			ar->PopObj();
		}
	}
	else
	{
		int fileVersion = 0;
			if (!ar->PushObj("PointLightComponent")) return;
				reg.emplace<PointLightComponent>(e);
				PointLightComponent* mc = reg.try_get<PointLightComponent>(e);
				*ar >> fileVersion;
				switch (fileVersion)
				{
				case 1:
				{
					ar->PushObj("Colour");
						*ar >> mc->colour;
					ar->PopObj();
				}
				default:

					break;
				}
				
			ar->PopObj();
	}
}

static void MetaReg(Comp::ComponentMeta* m)
{
	//using namespace entt::literals;
	//entt::meta_factory<MeshComponent> factory = entt::meta<MeshComponent>().type("MeshComponent"_hs);
	//factory.ctor<glm::vec3>()
	//	.data<nullptr, &col, entt::as_is_t>("mesh"_hs);
}

META_IMPL(PointLightComponent, MetaReg, SerializeC)

