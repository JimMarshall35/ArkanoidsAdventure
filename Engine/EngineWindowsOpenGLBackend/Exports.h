#pragma once
#pragma once
#ifdef ENGINE_WINDOWS_OPENGL_BACKEND_EXPORTS
#define WGL_BACKEND_API __declspec(dllexport)
#else
#define WGL_BACKEND_API __declspec(dllimport)
#endif

#include "IBackendApp.h"

extern "C"
{
	WGL_BACKEND_API BackendAPI GetApp();
}
