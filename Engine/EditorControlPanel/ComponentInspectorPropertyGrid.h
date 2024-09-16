#pragma once
#include "framework.h"
#include <string>
#include "pugixml.hpp"
#include <stdint.h>

class CComponentInspectorPropertyGrid : public CMFCPropertyGridCtrl
{
public:
	void OnNewComponentSelected(uint32_t entity, pugi::xml_node node);
protected:
	uint32_t m_hEntity = 0;
	pugi::xml_node m_Node;
};
