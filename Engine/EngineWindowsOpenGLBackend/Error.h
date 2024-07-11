#pragma once

#include "IBackendApp.h"

namespace Log
{
	void LogMsg(const BackendLog& error);
	void RegisterErrorHandler(BackendErrorHandlerFn handler);

};