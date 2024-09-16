#pragma once
#include "framework.h"
#include "pugixml.hpp"

class CEntityTreeCtrl : public CTreeCtrl
{
public:
	void OnNewSceneRecived(const pugi::xml_document& xmlScene);
};
