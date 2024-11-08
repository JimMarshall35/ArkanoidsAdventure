#pragma once
#include <string>

namespace EditorServer
{
	struct UploadMeshFileMsg
	{
		std::string assetFolderRelativePath;
	};

	struct UploadMeshFileMsg_Response
	{
		std::string loadedMeshXML; // empty string if fails
	};
}