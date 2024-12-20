#pragma once
#include "MessageTypes.h"
#include "NewEntityMessage.h"
#include "GetSceneXMLMessage.h"
#include "EditComponentMessage.h"
#include "EngineLogMsg.h"
#include "EngineCommandMsg.h"
#include <variant>
#include "SetGizmoEntityMessage.h"
#include "GetAssetsFolderPathMsg.h"
#include "UploadTextureFileMsg.h"
#include "UploadMeshFileMsg.h"
#include "SetPrefabPalette.h"

namespace EditorServer
{
	struct Msg
	{
		MsgType Type;
		std::variant<
			NewEntityMessage, NewEntityMessage_Response,
			GetSceneXmlMsg, GetSceneXmlMsg_Response,
			EditComponentMsg, EditComponentMsg_Response,
			EngineCmdMsg, EngineLogMsg,
			SetEntityGizmoMsg, ClearEntityGizmoMsg,
			SetGizmoOperationMsg,
			RequestAssetsFolderPath, RequestAssetsFolderPath_Response,
			UploadTextureFileMsg, UploadTextureFile_Response,
			UploadMeshFileMsg, UploadMeshFileMsg_Response,
			SetPrefabPaletteSlotMsg, SetPrefabPaletteSlotMsg_Response
		> Data;
	};

	Msg DeserializeMsg(unsigned char* data, size_t dataLen);
	unsigned char* SerializeMsg(const Msg& msg, size_t& outSize);
}