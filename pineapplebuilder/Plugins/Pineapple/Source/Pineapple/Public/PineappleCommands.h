// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PineappleStyle.h"

class FPineappleCommands : public TCommands<FPineappleCommands>
{
public:

	FPineappleCommands()
		: TCommands<FPineappleCommands>(TEXT("Pineapple"), NSLOCTEXT("Contexts", "Pineapple", "Pineapple Plugin"), NAME_None, FPineappleStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};