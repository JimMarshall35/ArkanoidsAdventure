#pragma once
#include "pch.h"
#include <memory>
#include <string>

namespace EditorServer
{
	struct NewEntityMessage
	{
		std::string xml;
	};

	struct NewEntityMessage_Response
	{
		bool bOK;
	};

}