#pragma once
#include "framework.h"
#include <string>
#include "pugixml.hpp"
#include <stdint.h>

class CComponentInspectorPropertyGrid : public CMFCPropertyGridCtrl
{
public:
	CComponentInspectorPropertyGrid(bool bSendMessageOnChange = true);
	void OnNewComponentSelected(uint32_t entity, pugi::xml_node node);
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const override;
	void UpdateComponent(size_t entity, pugi::xml_node newNode, const char* componentName);
protected:
	uint32_t m_hEntity = 0;
	pugi::xml_node m_Node;
	BOOL m_bSendMessageOnChange = TRUE;
};
