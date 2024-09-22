#include "SystemsReg.h"
#include "Scene.h"
#include "FrontendError.h"

void SystemsReg::Clear()
{
	//m_DrawFnPool.clea
}

HDrawSystem SystemsReg::RegisterDrawSystem(DrawSystem* ds)
{
	return RegisterDrawSystem(ds->GetFn(), ds->GetGroup(), ds->GetName());
}

HUpdateSystem SystemsReg::RegisterUpdateSystem(UpdateSystem* ds)
{
	return RegisterUpdateSystem(ds->GetFn(), ds->GetGroup(), ds->GetName());
}

HDrawSystem SystemsReg::RegisterDrawSystem(DrawSystemFn fn, DrawFunctionGroup g, const char* name)
{
	HDrawSystem d;
	DrawSystemPoolNode* fnNew;
	m_DrawFnPool.AllocateNew(d, fnNew);
	fnNew->fn = fn;
	EAssert(strlen(name) < 32);
	strcpy_s(fnNew->name, 32, name);
	m_DrawHandles[(size_t)g].push_back(d);
	return d;
}

HUpdateSystem SystemsReg::RegisterUpdateSystem(UpdateSystemFn fn, UpdateFunctionGroup g, const char* name)
{
	HUpdateSystem d;
	UpdateSystemPoolNode* fnNew;
	m_UpdateFnPool.AllocateNew(d, fnNew);
	fnNew->fn = fn;
	EAssert(strlen(name) < 32);
	strcpy_s(fnNew->name, 32, name);
	m_UpdateHandles[(size_t)g].push_back(d);
	return d;
}

void SystemsReg::UnRegisterDrawSystem(HDrawSystem fn, DrawFunctionGroup g)
{
	EVec<HDrawSystem>& hndls = m_DrawHandles[(size_t)g];
	auto itr = std::find(hndls.begin(), hndls.end(), fn);
	if (itr != hndls.end())
	{
		hndls.erase(itr);
		bool b = m_DrawFnPool.DeallocateObject(fn);
		EAssert(b);
	}
	else
	{
		Err::LogError("can't unregister draw system '%i', handle not valid", (u32)fn);
	}
}

void SystemsReg::UnRegisterUpdateSystem(HUpdateSystem fn, UpdateFunctionGroup g)
{
	EVec<HUpdateSystem>& hndls = m_UpdateHandles[(size_t)g];
	auto itr = std::find(hndls.begin(), hndls.end(), fn);
	if (itr != hndls.end())
	{
		hndls.erase(itr);
		bool b = m_UpdateFnPool.DeallocateObject(fn);
		EAssert(b);
	}
	else
	{
		Err::LogError("can't unregister update system '%i', handle not valid", (u32)fn);
	}
}

void SystemsReg::Update(double deltaT)
{
	Scn::Scene& scn = Scn::GetScene();
	for (u32 i = 0; i < (u32)UpdateFunctionGroup::Num; i++)
	{
		const EVec<HUpdateSystem>& grp = m_UpdateHandles[i];
		for (HUpdateSystem h : grp)
		{
			m_UpdateFnPool[h].fn(scn, deltaT);
		}
	}
	
}

void SystemsReg::Draw()
{
	Scn::Scene& scn = Scn::GetScene();
	for (u32 i = 0; i < (u32)DrawFunctionGroup::Num; i++)
	{
		const EVec<HDrawSystem>& grp = m_DrawHandles[i];
		for (HDrawSystem h : grp)
		{
			m_DrawFnPool[h].fn(scn);
		}
	}
}

SystemsRegGraph::SystemsRegGraph(Flow& f)
	:builder(&f)
{
}

SystemsRegGraph SystemsRegGraph::Bind(const Process& proc)
{
	Flow& f = builder->bind(proc.GetName());
	return SystemsRegGraph(f);
}

SystemsRegGraph SystemsRegGraph::ReadOnly(const Resource& res)
{
	return SystemsRegGraph(builder->ro(res.GetName()));
}

SystemsRegGraph SystemsRegGraph::ReadWrite(const Resource& res)
{
	return SystemsRegGraph(builder->rw(res.GetName()));
}

SystemsRegGraph SystemsRegGraph::Sync()
{
	return SystemsRegGraph(builder->sync());
}

ExecutionGraph SystemsRegGraph::Build()
{
	return builder->graph();
}

ExecutionGraph SystemsRegGraph::Test()
{
	Resource rInput("RInput");
	Process input("Input");
	Resource rPhysics("RPhysics");
	Process prePhysics("PrePhysics");
	Process postPhysics("PostPhysics");

	Bind(input)
		.ReadWrite(rInput)
	.Bind(prePhysics)
		.ReadOnly(rInput)
		.ReadWrite(rPhysics)
	.Bind(postPhysics)
		.ReadOnly(rPhysics);
	return builder->graph();
}

UpdateSystem::UpdateSystem(UpdateSystemFn fn, UpdateFunctionGroup group, const char* name)
	:Autolist(true), m_pFn(fn), m_nGroup(group), m_strName(name)
{
}

UpdateSystem* UpdateSystem::GetByName(const char* name)
{
	Autolist<UpdateSystem>* head = Autolist<UpdateSystem>::GetHead();
	while (head)
	{
		UpdateSystem* p = (UpdateSystem*)head;
		if (strcmp(p->GetName(), name) == 0)
		{
			return p;
		}
		head = head->GetNext();
	}
	return nullptr;
}

DrawSystem::DrawSystem(DrawSystemFn fn, DrawFunctionGroup group, const char* name)
	:Autolist(true), m_pFn(fn), m_nGroup(group), m_strName(name)
{
	
}

DrawSystem* DrawSystem::GetByName(const char* name)
{
	Autolist<DrawSystem>* head = Autolist<DrawSystem>::GetHead();
	while (head)
	{
		DrawSystem* p = (DrawSystem*)head;
		if (strcmp(p->GetName(), name) == 0)
		{
			return p;
		}
		head = head->GetNext();
	}
	return nullptr;
}
