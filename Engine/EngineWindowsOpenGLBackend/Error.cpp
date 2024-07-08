#include "Error.h"
namespace Error
{
	static BackendErrorHandlerFn gErrorHandler = nullptr;

	void ErrorMsg(const BackendError& error)
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