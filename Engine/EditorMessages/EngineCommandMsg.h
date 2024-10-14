#pragma once
#include <string>

namespace EditorServer
{
	/*
		sent by editor to game when a command is entered into the console
	*/
	struct EngineCmdMsg
	{
		std::string cmd;
	};
}