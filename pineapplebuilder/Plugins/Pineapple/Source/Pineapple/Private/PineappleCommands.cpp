// Copyright Epic Games, Inc. All Rights Reserved.

#include "PineappleCommands.h"

#define LOCTEXT_NAMESPACE "FPineappleModule"

void FPineappleCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Pineapple", "Bring up Pineapple window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
