// Copyright Epic Games, Inc. All Rights Reserved.

#include "PineappleEditorModeToolkit.h"
#include "PineappleEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "PineappleEditorModeToolkit"

FPineappleEditorModeToolkit::FPineappleEditorModeToolkit()
{
}

void FPineappleEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FPineappleEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FPineappleEditorModeToolkit::GetToolkitFName() const
{
	return FName("PineappleEditorMode");
}

FText FPineappleEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "PineappleEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
