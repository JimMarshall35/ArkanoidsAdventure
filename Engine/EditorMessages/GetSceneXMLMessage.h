#pragma once
#include "MessageTypes.h"
#include <string>
namespace EditorServer
{
	/*
		sent by editor to game when the serialized scene is requested
	*/
	struct GetSceneXmlMsg
	{

	};

	/*
		sent by game to editor in response to a GetSceneXmlMsg
	*/
	struct GetSceneXmlMsg_Response
	{
		std::string xml;
	};
}