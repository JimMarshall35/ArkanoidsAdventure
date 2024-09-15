#pragma once
#include "MessageTypes.h"
#include <string>
namespace EditorServer
{
	struct GetSceneXmlMsg
	{

	};

	struct GetSceneXmlMsg_Response
	{
		std::string xml;
	};
}