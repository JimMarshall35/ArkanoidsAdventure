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
