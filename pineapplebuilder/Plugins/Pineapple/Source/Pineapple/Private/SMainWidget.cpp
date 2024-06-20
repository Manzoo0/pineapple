// Fill out your copyright notice in the Description page of Project Settings.


#include "SMainWidget.h"
#include "SlateOptMacros.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/LODActor.h"
#include "Editor/GroupActor.h"
#include "IHierarchicalLODUtilities.h"
#include "HierarchicalLODUtilitiesModule.h"
#include "EngineUtils.h"
#include "Selection.h"

#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "SMainWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMainWidget::Construct(const FArguments& InArgs)
{	
	ChildSlot.HAlign(HAlign_Left).VAlign(VAlign_Top)
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SButton).OnClicked(this, &SMainWidget::btn_exclusiveselect_onclicked)[SNew(STextBlock).Text(LOCTEXT("btn_exclusiveselect", "Exclusive select"))]
			]
			+SVerticalBox::Slot().AutoHeight()
			[
				SNew(SButton).OnClicked(this, &SMainWidget::btn_tohlodproxy_onclicked)	[ SNew(STextBlock).Text(LOCTEXT("btn_tohlodproxy", "To HLOD Proxy")) ]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SMainWidget::btn_tohlodproxy_onclicked()
{
	USelection* Selection = GEditor->GetSelectedActors();
	TArray<AActor*> SelectedActors;
	for (FSelectionIterator Iter(*Selection); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor)
		{
			SelectedActors.Add(Actor);
		}
	}

	SetupHLODClusterForEachGroup(SelectedActors);

	return FReply::Handled();
}

FReply SMainWidget::btn_exclusiveselect_onclicked()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if (!EditorActorSubsystem)
	{
		return FReply::Unhandled();
	}

	USelection* Selection = GEditor->GetSelectedActors();
	TArray<AActor*> ExclusiveSelectActors;
	for (FSelectionIterator Iter(*Selection); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor)
		{
			//todo : WasRecentlyRendered not render check precisely, need to frustum culling check manually.
			if (Actor->WasRecentlyRendered(0.1f))
			{
				ExclusiveSelectActors.Add(Actor);
			}
		}
	}
	
	//set selection
	EditorActorSubsystem->SetSelectedLevelActors(ExclusiveSelectActors);

	return FReply::Handled();
}

void SMainWidget::SetupHLODClusterForEachGroup(TArray<AActor*> InActors, int32 InLODLevel)
{
	//get world
	UWorld* lWorld = nullptr;
	if (InActors.Num() > 0)
	{
		if (IsValid(InActors[0]))
		{
			lWorld = InActors[0]->GetWorld();
		}
	}

	//invalid world. abort.
	if (!IsValid(lWorld))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SMainWidget::SetupHLODClusterForEachGroup] Invalid world"));
		return;
	}

	//invalid worldsetting. abort.
	if (!IsValid(lWorld->GetWorldSettings()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SMainWidget::SetupHLODClusterForEachGroup] Invalid world settings"));
		return;
	}

	if (!lWorld->GetWorldSettings()->HLODSetupAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SMainWidget::SetupHLODClusterForEachGroup] Invalid HLODSetupAsset"));
		return;
	}

	//HLOD util
	FHierarchicalLODUtilitiesModule& HierarchicalLODUtilitiesModule = FModuleManager::LoadModuleChecked<FHierarchicalLODUtilitiesModule>("HierarchicalLODUtilities");
	IHierarchicalLODUtilities* HierarchicalLODUtilities = HierarchicalLODUtilitiesModule.GetUtilities();
	if (!HierarchicalLODUtilities)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SMainWidget::SetupHLODClusterForEachGroup] Invalid HierarchicalLODUtilities"));
		return;
	}

	//process grouped actors
	const TArray<AActor*> GroupParents = InActors.FilterByPredicate([](const AActor* A) { return A->IsA<AGroupActor>(); });
	for (AActor* i : GroupParents)
	{
		if (IsValid(i))
		{
			AGroupActor* GroupActor = Cast<AGroupActor>(i);
			if (GroupActor)
			{
				TArray<AActor*> GroupedActors;
				GroupActor->GetGroupActors(GroupedActors);

				//filter AStaticMeshActor
				GroupedActors.RemoveAllSwap([](AActor* A) { return !A->IsA<AStaticMeshActor>(); });
				const bool bIsAlreadyClustered = GroupedActors.ContainsByPredicate([HierarchicalLODUtilities](const AActor* A) { return HierarchicalLODUtilities->IsActorClustered(A); });
				if (bIsAlreadyClustered)
				{
					GroupActor->ForEachActorInGroup([HierarchicalLODUtilities](AActor* InGroupedActor, AGroupActor* InGroupActor)
						{
							//cluster info debug log
							if (InGroupedActor->IsA<AStaticMeshActor>())
							{
								//InGroupedActor = member actor, InGroupActor = Group
								ALODActor* ParentClusterActor = HierarchicalLODUtilities->GetParentLODActor(InGroupedActor);
								FString ParentClusterName = ParentClusterActor ? ParentClusterActor->GetActorLabel() : TEXT("None");

								UE_LOG(LogTemp, Warning, TEXT("[UP2EditorFunctionLibrary::BuildHLODCluster] Actor=%s in Cluster=%s"), *InGroupedActor->GetActorLabel(), *ParentClusterName);
							}
						}
					);
				}
				else
				{
					//create new HLOD cluster with current group
					HierarchicalLODUtilities->CreateNewClusterFromActors(lWorld, lWorld->GetWorldSettings(), GroupedActors, InLODLevel);
				}
			}
		}
	}

	//process non grouped actors (single actor hlod cluster)
	const TArray<AActor*> NomadActors = InActors.FilterByPredicate([](const AActor* A) { return A->IsA<AStaticMeshActor>() && !A->GroupActor; });
	for (AActor* i : NomadActors)
	{
		if (IsValid(i))
		{
			if (HierarchicalLODUtilities->IsActorClustered(i))
			{
				ALODActor* ParentClusterActor = HierarchicalLODUtilities->GetParentLODActor(i);
				FString ParentClusterName = ParentClusterActor ? ParentClusterActor->GetActorLabel() : TEXT("None");
				UE_LOG(LogTemp, Warning, TEXT("[UP2EditorFunctionLibrary::BuildHLODCluster] Actor=%s in Cluster=%s"), *i->GetActorLabel(), *ParentClusterName);
			}
			else
			{
				//create new HLOD cluster with current Actor
				HierarchicalLODUtilities->CreateNewClusterFromActors(lWorld, lWorld->GetWorldSettings(), TArray<AActor*>({ i }), InLODLevel);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE