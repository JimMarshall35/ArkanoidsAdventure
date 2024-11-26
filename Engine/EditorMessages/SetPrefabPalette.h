#pragma once
#include <string>

namespace EditorServer
{
	struct SetPrefabPaletteSlotMsg
	{
		char slot = 0;
		std::string xml;
	};

	struct SetPrefabPaletteSlotMsg_Response
	{
		bool bSuccess = false;
	};

}