#pragma once
#include "ExportMacro.h"
#include "EngineLib.h"
struct BackendAPI;

struct EngineInitArgs
{
	EString CommandLineArgs;
	EWString BackEndRendererDLLPath;
	EString InitialScenePath = "";
};

namespace  Engine
{
	void ENGINE_FRONTEND_API Run(const EngineInitArgs& args);
	const BackendAPI& GetAPI();
	void GetWindowWidthAndHeight(int& pxW, int& pxH);
};


