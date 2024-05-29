// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/MergeHelper.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "Engine/World.h"

// localization namespace
#define LOCTEXT_NAMESPACE "PineappleMergeTool"

UInteractiveTool* UMergeHelper::BuildTool(const FToolBuilderState& SceneState) const
{
	UMergeHelperTool* NewTool = NewObject<UMergeHelperTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UMergeHelperProperties::UMergeHelperProperties()
{

}


UMergeHelperTool::UMergeHelperTool()
{	
}

void UMergeHelperTool::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UMergeHelperTool::Setup()
{
	UInteractiveTool::Setup();
	UE_LOG(LogTemp, Warning, TEXT("UMergeHelperTool::Setup"));

	USelection::SelectionChangedEvent.AddUObject(this, &UMergeHelperTool::OnLevelSelectionChanged);

	Properties = NewObject<UMergeHelperProperties>(this);
	AddToolPropertySource(Properties);
}

void UMergeHelperTool::Shutdown(EToolShutdownType ShutdownType)
{
	UInteractiveTool::Shutdown(ShutdownType);

	USelection::SelectionChangedEvent.RemoveAll(this);
}

void UMergeHelperTool::OnLevelSelectionChanged(UObject* Obj)
{
	TArray<AActor*> SelectedActors;
	GetSelectedActors(SelectedActors);
}

void UMergeHelperTool::GetSelectedActors(TArray<AActor*>& outSelectedActors)
{
	USelection* Selection = GEditor->GetSelectedActors();
	Selection->GetSelectedObjects<AActor>(outSelectedActors);

	for (AActor* SelectedActor : outSelectedActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedActor->GetActorLabel());
	}
}

#undef LOCTEXT_NAMESPACE