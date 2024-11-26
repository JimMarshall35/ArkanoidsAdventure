#include "ComponentReg.h"
#include <unordered_map>
#include "FrontendError.h"
#include "XMLArchive.h"
#include "Scene.h"

static EMap<EString, Comp::ComponentMeta*> gNameMap;

Comp::ComponentMeta::ComponentMeta()
	: Autolist<ComponentMeta>(true)
{
}

Comp::ComponentMeta::~ComponentMeta()
{
}

Comp::ComponentMeta* Comp::ComponentMeta::FindByName(const char* name)
{
	EString s(name);
	if (gNameMap.find(s) != gNameMap.end())
	{
		return gNameMap[s];
	}
	else
	{
		Err::LogError("[ComponentMeta::FindByName] Component with name '%s' not found.", name);
		return nullptr;
	}
}

EString Comp::ComponentMeta::GetDefaultComponentXMLString()
{
	XMLArchive ar("", true, false);
	SerializeDefaultComponent(&ar);
	return ar.AsString();
}

void Comp::ComponentMeta::SerializeDefaultComponent(IArchive* ar)
{
	EntityReg reg;
	Entity e = reg.create();
	EmplaceDefaultComponent(reg, e);
	Serialize(ar, e, reg);
}

void Comp::Init()
{
	Autolist<ComponentMeta>* head = Autolist<ComponentMeta>::GetHead();
	Scn::Scene& s = Scn::GetScene();
	while (head)
	{
		ComponentMeta* pMeta = (ComponentMeta*)head;
		EString name = pMeta->GetName();
		gNameMap[name] = pMeta;
		head = head->GetNext();
		pMeta->RegisterListeners(s.entities.GetReg());
	}
}

const EMap<EString, Comp::ComponentMeta*>& Comp::GetComponentMetaMap()
{
	return gNameMap;
}
