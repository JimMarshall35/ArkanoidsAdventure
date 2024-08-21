#pragma once
#include "EngineLib.h"
#include "Autolist.h"
#include <entt/entity/entity.hpp>
class IArchive;

namespace Comp
{
	class ComponentMeta : public Autolist<ComponentMeta>
	{
	public:
		ComponentMeta() : Autolist<ComponentMeta>(true){}
		virtual void Meta() = 0;
		virtual void Serialize(IArchive* ar, Entity e, EntityReg& reg) = 0;
	};
}
#define META_DECL(ckassName)\
	Comp::ComponentMeta* ckassName##_GetMeta();

#define META_IMPL(className, metaFn, SerializeFn) class className##_Meta : public Comp::ComponentMeta {\
	public:\
		virtual void Meta() override {\
			metaFn((Comp::ComponentMeta*)this);\
		}\
		virtual void Serialize(IArchive* ar, Entity e, EntityReg& reg) override {\
			SerializeFn((Comp::ComponentMeta*)this, ar, e, reg);\
		}\
	};\
	className##_Meta className##_Meta_Instance;\
	Comp::ComponentMeta* className##_GetMeta() { return (Comp::ComponentMeta*)&className##_Meta_Instance; }
	
