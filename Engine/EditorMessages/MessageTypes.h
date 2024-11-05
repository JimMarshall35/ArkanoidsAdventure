#pragma once
#include <stdint.h>
namespace EditorServer
{
	enum class MsgType : uint8_t
	{
		NewEntity, NewEntity_Response,
		GetSceneXML, GetSceneXML_Response,
		EditComponent, EditComponent_Response,
		EngineCmd, EngineLog,
		SetEntityGizmo,
		ClearGizmoEntity,
		SetGizmoOperation,
		RequestAssetsFolderPath, RequestAssetsFolderPath_Response,
		UploadTextureFile, UploadTextureFile_Response
	};
}
