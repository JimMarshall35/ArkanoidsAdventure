#pragma once
#ifdef ENGINE_WINDOWS_VULKAN_BACKEND_EXPORTS
#define WV_BACKEND_API __declspec(dllexport)
#else
#define WV_BACKEND_API __declspec(dllimport)
#endif

#include "IBackendApp.h"

extern "C"
{
	WV_BACKEND_API BackendAPI GetApp();
}
