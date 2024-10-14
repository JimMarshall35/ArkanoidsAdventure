#pragma once
#include <stdio.h>
#include <string>

namespace EditorServer
{
	/*
		- Sent by editor to game upon editing components with inspector
		- Sent by game to editor upon moving entities with gizmo, and possibly other cases in future for other components
	*/
	struct EditComponentMsg
	{
		EditComponentMsg() {}
		EditComponentMsg(size_t entity, const char* xml)
			:entity(entity), newComponentXml(xml){}

		size_t entity;
		std::string newComponentXml;
	};
	/*
		unused
	*/
	struct EditComponentMsg_Response
	{
		bool bSucceeded;
	};
}
