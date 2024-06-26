// Fill out your copyright notice in the Description page of Project Settings.


#include "PineappleCore.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/LODActor.h"
#include "Editor/GroupActor.h"
#include "IHierarchicalLODUtilities.h"
#include "HierarchicalLODUtilitiesModule.h"
#include "EngineUtils.h"
#include "Selection.h"
#include "Subsystems/EditorActorSubsystem.h"

TArray<TObjectPtr<AActor>> Pineapple::GetSelectedActors()
{
	TArray<TObjectPtr<AActor>> OutActors;

	USelection* Selection = GEditor->GetSelectedActors();
	for (FSelectionIterator Iter(*Selection); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		if (Actor)
		{
			OutActors.Add(Actor);
		}
	}

	return OutActors;
}

TArray<TObjectPtr<ALODActor>> Pineapple::GetAllLODActorsInLevel()
{
	TArray<TObjectPtr<ALODActor>> OutActors;

	UWorld* World = GEditor->GetEditorWorldContext(false).World();
	if (World)
	{
		//gather ALODActors (built HLOD cluster)
		for (TActorIterator<ALODActor> It(World); It; ++It)
		{
			//gathering ALODActor
			if (*It)
			{
				OutActors.Add(*It);
			}
		}
	}

	return OutActors;
}

void Pineapple::CreateNewClusterFromActors(TArray<TObjectPtr<AActor>> InActors, int32 InLODLevel /*= 0*/)
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

bool Pineapple::GenerateShadowproxyFromLODActors()
{
	UWorld* World = GEditor->GetEditorWorldContext(false).World();
	if (!World)
	{
		return false;
	}

	//gather ALODActors (built HLOD cluster)
	TArray<TObjectPtr<ALODActor>> LODActors;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		//Destroy deprecated shadow proxy actors
		if (*It && (*It)->ActorHasTag(NAME_SHADOWPROXY))
		{
			(*It)->Destroy();
			continue;
		}

		//gathering ALODActor
		if (ALODActor* LODActor = Cast<ALODActor>(*It))
		{
			LODActors.Add(LODActor);
		}
	}

	const int32 NumNeedBuild = LODActors.FilterByPredicate([](TObjectPtr<ALODActor> A) { return A == nullptr || A->IsBuilt() == false; }).Num();
	if (NumNeedBuild > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%d] HLOD cluster(s) need to build."), NumNeedBuild);
		return false;
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (auto LODActor = LODActors.CreateIterator(); LODActor; ++LODActor)
	{
		if (*LODActor)
		{
			//Spawn shadow proxy actor
			FTransform SpawnTransform = (*LODActor)->GetActorTransform();
			AStaticMeshActor* ShadowProxy = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform, ActorSpawnParameters);
			if (ShadowProxy)
			{
				//set Folder path in SceneOutliner
				ShadowProxy->SetFolderPath(NAME_SHADOWPROXY);

				//set actor editor display name(EDITOR ONLY)
				ShadowProxy->SetActorLabel(FString(NAME_SHADOWPROXY).Append(TEXT("_")).Append((*LODActor)->GetActorLabel()));

				//setup tag
				ShadowProxy->Tags.Add(NAME_SHADOWPROXY);

				//setup primitive component				
				const UStaticMeshComponent* const SourcePrimitiveComponent = (*LODActor)->GetStaticMeshComponent();
				UStaticMeshComponent* TargetPrimitiveComponent = ShadowProxy->GetStaticMeshComponent();
				TargetPrimitiveComponent->SetStaticMesh(SourcePrimitiveComponent->GetStaticMesh());
				TargetPrimitiveComponent->SetRenderInMainPass(false);
				TargetPrimitiveComponent->SetRenderInDepthPass(false);
				TargetPrimitiveComponent->SetVisibleInRayTracing(false);
				TargetPrimitiveComponent->bVisibleInReflectionCaptures = false;
				TargetPrimitiveComponent->bVisibleInRealTimeSkyCaptures = false;
				TargetPrimitiveComponent->SetReceivesDecals(false);
			}

			//Disable shadow for source actors
			for (auto SubActor = (*LODActor)->SubActors.CreateIterator(); SubActor; ++SubActor)
			{
				AStaticMeshActor* SMA = Cast<AStaticMeshActor>(*SubActor);
				{
					SMA->GetStaticMeshComponent()->SetCastShadow(false);
				}
			}
		}
	}

	return true;
}

void Pineapple::ExclusiveSelect()
{
	UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	if (!EditorActorSubsystem)
	{
		return;
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
}