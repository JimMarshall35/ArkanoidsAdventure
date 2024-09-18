#include "ComponentReg.h"
#include <unordered_map>
#include "FrontendError.h"
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
void Comp::Init()
{
	Autolist<ComponentMeta>* head = Autolist<ComponentMeta>::GetHead();
	while (head)
	{
		ComponentMeta* pMeta = (ComponentMeta*)head;
		EString name = pMeta->GetName();
		gNameMap[name] = pMeta;
		head = head->GetNext();
	}
}