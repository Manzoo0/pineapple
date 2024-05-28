// Copyright Epic Games, Inc. All Rights Reserved.

#include "PineappleModule.h"
#include "PineappleEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "PineappleModule"

void FPineappleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FPineappleEditorModeCommands::Register();
}

void FPineappleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FPineappleEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPineappleModule, PineappleEditorMode)