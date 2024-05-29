// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * This class contains info about the full set of commands used in this editor mode.
 */
class FPineappleEditorModeCommands : public TCommands<FPineappleEditorModeCommands>
{
public:
	FPineappleEditorModeCommands();

	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	//TSharedPtr<FUICommandInfo> SimpleTool;
	//TSharedPtr<FUICommandInfo> InteractiveTool;
	TSharedPtr<FUICommandInfo> UIC_MergeHelper;

protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
