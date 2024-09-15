#pragma once
#include <stdint.h>
namespace EditorServer
{
	enum class MsgType : uint8_t
	{
		NewEntity, NewEntity_Response,
		GetSceneXML, GetSceneXML_Response
	};
}
