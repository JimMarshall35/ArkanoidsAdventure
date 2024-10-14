#pragma once
#include "framework.h"
#include "pugixml.hpp"

class CEntityTreeCtrl : public CTreeCtrl
{
public:
	void OnNewSceneRecived(const pugi::xml_node& xmlScene);
	void UpdateComponent(size_t entity, pugi::xml_node newNode, const char* componentName);
};
