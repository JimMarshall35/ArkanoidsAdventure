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
}
