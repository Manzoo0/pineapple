// Copyright Epic Games, Inc. All Rights Reserved.

#include "PineappleEditorMode.h"
#include "PineappleEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "PineappleEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/PineappleSimpleTool.h"
#include "Tools/PineappleInteractiveTool.h"

// step 2: register a ToolBuilder in FPineappleEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "PineappleEditorMode"

const FEditorModeID UPineappleEditorMode::EM_PineappleEditorModeId = TEXT("EM_PineappleEditorMode");

FString UPineappleEditorMode::SimpleToolName = TEXT("Pineapple_ActorInfoTool");
FString UPineappleEditorMode::InteractiveToolName = TEXT("Pineapple_MeasureDistanceTool");


UPineappleEditorMode::UPineappleEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UPineappleEditorMode::EM_PineappleEditorModeId,
		LOCTEXT("ModeName", "Pineapple"),
		FSlateIcon(),
		true);
}


UPineappleEditorMode::~UPineappleEditorMode()
{
}


void UPineappleEditorMode::ActorSelectionChangeNotify()
{
}

void UPineappleEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FPineappleEditorModeCommands& SampleToolCommands = FPineappleEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UPineappleSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UPineappleInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UPineappleEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FPineappleEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UPineappleEditorMode::GetModeCommands() const
{
	return FPineappleEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
