#include "Error.h"
namespace Log
{
	static BackendErrorHandlerFn gErrorHandler = nullptr;

	void LogMsg(const BackendLog& error)
	{
		if (gErrorHandler)
		{
			gErrorHandler(error);
		}
	}

	void RegisterErrorHandler(BackendErrorHandlerFn handler)
	{
		gErrorHandler = handler;
	}

}