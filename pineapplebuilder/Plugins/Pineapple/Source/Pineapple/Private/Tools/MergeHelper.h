// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "MultiSelectionTool.h"

#include "MergeHelper.generated.h"

/**
 * 
 */
UCLASS()
class UMergeHelper : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class PINEAPPLE_API UMergeHelperProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:

	UMergeHelperProperties();
};


UCLASS()
class PINEAPPLE_API UMergeHelperTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	UMergeHelperTool();
	virtual void SetWorld(UWorld* World);
	virtual void Setup() override;
	virtual void Shutdown(EToolShutdownType ShutdownType) override;

	void OnLevelSelectionChanged(UObject* Obj);

	void GetSelectedActors(TArray<AActor*>& outSelectedActors);

protected:
	UPROPERTY()
	TObjectPtr<UMergeHelperProperties> Properties;

	/** target World we will raycast into to find actors */
	UWorld* TargetWorld;
};