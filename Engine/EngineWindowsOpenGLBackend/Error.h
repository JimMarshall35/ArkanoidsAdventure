#pragma once

#include "IBackendApp.h"

namespace Error
{
	void ErrorMsg(const BackendError& error);
	void RegisterErrorHandler(BackendErrorHandlerFn handler);

};