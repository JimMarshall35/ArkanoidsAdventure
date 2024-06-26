#pragma once
#ifdef ENGINEFRONTEND_EXPORTS
#define ENGINE_FRONTEND_API __declspec(dllexport)
#else
#define ENGINE_FRONTEND_API __declspec(dllimport)

#endif

#include "EngineLib.h"
class BackendAPI;

struct EngineInitArgs
{
	EString CommandLineArgs;
	EWString BackEndRendererDLLPath;
};

namespace  Engine
{
	void ENGINE_FRONTEND_API Run(const EngineInitArgs& args);

};


