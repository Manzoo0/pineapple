// Copyright Epic Games, Inc. All Rights Reserved.

#include "Pineapple.h"
#include "PineappleStyle.h"
#include "PineappleCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName PineappleTabName("Pineapple");

#define LOCTEXT_NAMESPACE "FPineappleModule"

void FPineappleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPineappleStyle::Initialize();
	FPineappleStyle::ReloadTextures();

	FPineappleCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPineappleCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPineappleModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPineappleModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PineappleTabName, FOnSpawnTab::CreateRaw(this, &FPineappleModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPineappleTabTitle", "Pineapple"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FPineappleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPineappleStyle::Shutdown();

	FPineappleCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PineappleTabName);
}

TSharedRef<SDockTab> FPineappleModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FPineappleModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Pineapple.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FPineappleModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(PineappleTabName);
}

void FPineappleModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPineappleCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPineappleCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPineappleModule, Pineapple)