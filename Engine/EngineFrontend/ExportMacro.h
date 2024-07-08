#pragma once

#ifdef ENGINEFRONTEND_EXPORTS
#define ENGINE_FRONTEND_API __declspec(dllexport)
#else
#define ENGINE_FRONTEND_API __declspec(dllimport)
#endif
