#pragma once
#include "EngineLib.h"
#include "ComponentReg.h"
#include "SystemsReg.h"
#define EDITOR_CONTROLS_SYSTEM_NAME "Editor Controls"
extern UpdateSystem gEditorControlsSystem;


bool SetEntityPaletteEntry(const char* e, int paletteIndex);