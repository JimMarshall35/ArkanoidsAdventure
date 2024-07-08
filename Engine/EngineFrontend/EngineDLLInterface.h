#pragma once
#include "ExportMacro.h"
#include "EngineLib.h"
struct BackendAPI;

struct EngineInitArgs
{
	EString CommandLineArgs;
	EWString BackEndRendererDLLPath;
};

namespace  Engine
{
	void ENGINE_FRONTEND_API Run(const EngineInitArgs& args);
	const BackendAPI& GetAPI();
};


