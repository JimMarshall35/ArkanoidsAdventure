#pragma once
#include "MessageTypes.h"
#include "NewEntityMessage.h"
#include "GetSceneXMLMessage.h"
#include <variant>

namespace EditorServer
{
	struct Msg
	{
		MsgType Type;
		std::variant<
			NewEntityMessage, NewEntityMessage_Response,
			GetSceneXmlMsg, GetSceneXmlMsg_Response
		> Data;
	};

	Msg DeserializeMsg(unsigned char* data, size_t dataLen);
	unsigned char* SerializeMsg(const Msg& msg, size_t& outSize);
}