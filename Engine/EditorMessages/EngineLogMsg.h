#pragma once
#include <string>
#include <stdint.h>

namespace EditorServer
{
	enum class LogSeverity : std::uint8_t
	{
		Info,
		Warning,
		Error,
		Unknown
	};

	enum class LogSource : std::uint8_t
	{
		Backend,
		Frontend
	};

	/*
		Sent by game to editor when a message is logged
	*/
	struct EngineLogMsg
	{
		LogSource src;
		LogSeverity severity;
		std::string msg;
	};
}