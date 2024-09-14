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

	void ReportError(const FrontendLog& err);
	void ReportError(FrontendErrorSeverity es, const char* fmt, ...);

	void LogInfo(const char* fmt, ...);
	void LogWarning(const char* fmt, ...);
	void LogError(const char* fmt, ...);

}
