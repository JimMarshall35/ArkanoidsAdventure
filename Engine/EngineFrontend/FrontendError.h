#pragma once
#include "EngineLib.h"

struct BackendLog;
namespace Err
{
	enum class FrontendErrorSeverity
	{
		Info,
		Warning,
		Error,
		Unknown,
		Num
	};

	struct FrontendLog
	{
		EString Msg;
		FrontendErrorSeverity Severity;
	};

	//void ReportError(const FrontendLog& err);
	//void ReportError(FrontendErrorSeverity es, const char* fmt, ...);

	void LogInfo(const char* fmt, ...);
	void LogWarning(const char* fmt, ...);
	void LogError(const char* fmt, ...);

	// don't send a message to the editor process when called, only the local game process logs.
	// used by the editor server itself for logging as a feedback loop can occur
	void LogInfoLocal(const char* fmt, ...);
	void LogWarningLocal(const char* fmt, ...);
	void LogErrorLocal(const char* fmt, ...);

}
