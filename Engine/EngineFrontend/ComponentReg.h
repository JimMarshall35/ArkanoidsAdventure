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
		ComponentMeta();
		~ComponentMeta();
		virtual void Meta() = 0;
		virtual void Serialize(IArchive* ar, Entity e, EntityReg& reg) = 0;
		virtual void RegisterListeners(EntityReg& reg) = 0;
		virtual const char* GetName() { return ""; }
		static ComponentMeta* FindByName(const char* name);
	};
	void Init();
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
		virtual void RegisterListeners(EntityReg& reg) {}\
		virtual const char* GetName() override { return #className ; }\
	};\
	className##_Meta className##_Meta_Instance;\
	Comp::ComponentMeta* className##_GetMeta() { return (Comp::ComponentMeta*)&className##_Meta_Instance; }
	
// Listener type for ConstructFn, DestructFn, UpdateFn: void(entt::registry &, entt::entity);
#define META_IMPL_EX(className, metaFn, SerializeFn, ConstructFn, DestructFn, UpdateFn) class className##_Meta : public Comp::ComponentMeta {\
	public:\
		virtual void Meta() override {\
			metaFn((Comp::ComponentMeta*)this);\
		}\
		virtual void Serialize(IArchive* ar, Entity e, EntityReg& reg) override {\
			SerializeFn((Comp::ComponentMeta*)this, ar, e, reg);\
		}\
		virtual void RegisterListeners(EntityReg& reg) {\
			reg.on_construct<##className##>().connect<&##ConstructFn##>();\
			reg.on_destroy<##className##>().connect<&##DestructFn##>();\
			reg.on_update<##className##>().connect<&##UpdateFn##>();\
		}\
		virtual const char* GetName() override { return #className ; }\
	};\
	className##_Meta className##_Meta_Instance;\
	Comp::ComponentMeta* className##_GetMeta() { return (Comp::ComponentMeta*)&className##_Meta_Instance; }