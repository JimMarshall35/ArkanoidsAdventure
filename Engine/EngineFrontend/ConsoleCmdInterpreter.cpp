#include "ConsoleCmdInterpreter.h"
#include "EditorServer.h"
#include "EngineLib.h"
#include "FrontendError.h"

void CCmd::DoCmd(const char* text)
{
	// stub
	EString response = "command OK";
	Err::LogInfo("Command '%s' done ok", text);
}
