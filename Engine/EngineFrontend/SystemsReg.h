#pragma once
#include "EngineLib.h"
#include "ObjectPool.h"
#include "ExportMacro.h"
#include "Autolist.h"

// TEMPORARY CRUDE IMPLEMENTATION
namespace Scn
{
	struct Scene;
}
typedef void (*UpdateSystemFn)(Scn::Scene& s, double deltaT);
typedef void (*DrawSystemFn)(Scn::Scene& s);

typedef Handle_t HUpdateSystem;
typedef Handle_t HDrawSystem;

enum class UpdateFunctionGroup : u32
{
	PreInput,
	PrePhysics,
	PostPhysics,
	Num
};

enum class DrawFunctionGroup : u32
{
	Draw1,
	Draw2,
	Num
};

class ENGINE_FRONTEND_API UpdateSystem : Autolist<UpdateSystem>
{
public:
	UpdateSystem(UpdateSystemFn fn, UpdateFunctionGroup group, const char* name);
	static UpdateSystem* GetByName(const char* name);
	const char* GetName() { return m_strName.c_str(); }
	UpdateSystemFn GetFn() { return m_pFn; }
	UpdateFunctionGroup GetGroup() { return m_nGroup; }

private:
	UpdateSystemFn m_pFn;
	UpdateFunctionGroup m_nGroup;
	EString m_strName;
};

class ENGINE_FRONTEND_API DrawSystem : Autolist<DrawSystem>
{
public:
	DrawSystem(DrawSystemFn fn, DrawFunctionGroup group, const char* name);
	static DrawSystem* GetByName(const char* name);
	const char* GetName() { return m_strName.c_str(); }
	DrawSystemFn GetFn() { return m_pFn; }
	DrawFunctionGroup GetGroup() { return m_nGroup; }

private:
	DrawSystemFn m_pFn;
	DrawFunctionGroup m_nGroup;
	EString m_strName;
};

class ENGINE_FRONTEND_API SystemsReg
{
public:
	void Clear();
	HDrawSystem RegisterDrawSystem(DrawSystem* ds);
	HUpdateSystem RegisterUpdateSystem(UpdateSystem* ds);

	HDrawSystem RegisterDrawSystem(DrawSystemFn fn, DrawFunctionGroup g, const char* name);
	HUpdateSystem RegisterUpdateSystem(UpdateSystemFn fn, UpdateFunctionGroup g, const char* name);
	void UnRegisterDrawSystem(HDrawSystem fn, DrawFunctionGroup g);
	void UnRegisterUpdateSystem(HUpdateSystem fn, UpdateFunctionGroup g);

	void Update(double deltaT);
	void Draw();
private:
	struct UpdateSystemPoolNode
	{
		UpdateSystemFn fn;
		char name[32];
	};
	struct DrawSystemPoolNode
	{
		DrawSystemFn fn;
		char name[32];
	};
	//EVec<Handle_t> m_UpdateHandles;
	EVec<HUpdateSystem> m_UpdateHandles[(u32)UpdateFunctionGroup::Num];
	EVec<HDrawSystem> m_DrawHandles[(u32)DrawFunctionGroup::Num];
	EObjectPool<UpdateSystemPoolNode, 128> m_UpdateFnPool;
	EObjectPool<DrawSystemPoolNode, 128> m_DrawFnPool;

};

class Process
{
public:
	Process(const char* name) : m_hsName(name) {}
	entt::hashed_string GetName() const { return m_hsName; }
private:
	entt::hashed_string m_hsName;
};

class Resource
{
public:
	Resource(const char* name) : m_hsName(name) {}
	entt::hashed_string GetName() const { return m_hsName; }
private:
	entt::hashed_string m_hsName;
};

typedef entt::flow Flow;
typedef entt::adjacency_matrix<entt::directed_tag> ExecutionGraph;

class SystemsRegGraph
{
public:
	SystemsRegGraph(Flow& f);
	SystemsRegGraph Bind(const Process& proc);
	SystemsRegGraph ReadOnly(const Resource& proc);
	SystemsRegGraph ReadWrite(const Resource& proc);
	SystemsRegGraph Sync();
	ExecutionGraph Build();
	ExecutionGraph Test();
private:
	Flow* builder;

};