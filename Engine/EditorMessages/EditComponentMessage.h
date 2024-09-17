#pragma once
#include <stdio.h>
#include <string>

namespace EditorServer
{
	struct EditComponentMsg
	{
		EditComponentMsg() {}
		EditComponentMsg(size_t entity, const char* xml)
			:entity(entity), newComponentXml(xml){}

		size_t entity;
		std::string newComponentXml;
	};
	struct EditComponentMsg_Response
	{
		bool bSucceeded;
	};
}
