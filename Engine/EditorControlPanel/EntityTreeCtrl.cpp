#include "pch.h"
#include "EntityTreeCtrl.h"
#include "StringHelpers.h"

void CEntityTreeCtrl::OnNewSceneRecived(const pugi::xml_node& xmlScene)
{
	using namespace pugi;
	static_assert(sizeof(xml_node) == sizeof(DWORD_PTR));

	VERIFY(DeleteAllItems());
	if (xml_node ents = xmlScene.child("Entities"))
	{
		for (xml_node ent : ents)
		{
			ASSERT(strcmp(ent.name(), "Entity") == 0);
			if (xml_attribute a = ent.attribute("u32Val"))
			{
				std::uint32_t id = atoi(a.value());
				CString st;
				st.Format(_T("Entity (ID: %i)"), id);
				HTREEITEM hEntity = InsertItem(st);
				SetItemData(hEntity, (DWORD_PTR)id);
				for (xml_node prop : ent)
				{
					std::wstring ws = s2ws(prop.name());
					HTREEITEM hProp = InsertItem(ws.c_str(), hEntity);
					DWORD_PTR data = 0;
					memcpy(&data, &prop, sizeof(DWORD_PTR)); // lol 
					SetItemData(hProp, data);
				}
			}
		}
	}
}

void CEntityTreeCtrl::UpdateComponent(size_t entity, pugi::xml_node newNode, const char* componentName)
{
	static_assert(sizeof(pugi::xml_node) == sizeof(DWORD_PTR));
	HTREEITEM item = GetFirstVisibleItem();
	do
	{
		// find entity with changed component
		size_t ent = (size_t)GetItemData(item);
		if (ent == entity)
		{
			item = GetChildItem(item);
			do
			{
				// find changed component on entity
				DWORD_PTR componentData = GetItemData(item);
				pugi::xml_node asNode;
				memcpy(&asNode, &componentData, sizeof(pugi::xml_node));
				// the fact we can still read the old nodes name here when its been removed from the master xml document with remove_child means this is surely leaking memory ?!
				// or does it...
				const char* name = asNode.name();
				if (strcmp(name, componentName) == 0)
				{
					// update item data
					DWORD_PTR newNodeDword;
					memcpy(&newNodeDword, &newNode, sizeof(DWORD_PTR));
					SetItemData(item, newNodeDword);
					return;
				}
			} while (item = GetNextSiblingItem(item));
		}
	} while (item = GetNextSiblingItem(item));
	ASSERT(false);
}
