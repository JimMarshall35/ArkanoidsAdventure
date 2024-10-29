#pragma once
#include <string>
namespace EditorServer
{
	struct RequestAssetsFolderPath{};

	struct RequestAssetsFolderPath_Response
	{
		std::string absolutePath;
	};
}