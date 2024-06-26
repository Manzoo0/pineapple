// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PineappleCore.generated.h"

#define NAME_SHADOWPROXY TEXT("SHADOWPROXY")

class ALODActor;

namespace Pineapple
{
	TArray<TObjectPtr<AActor>> GetSelectedActors();
	TArray<TObjectPtr<ALODActor>> GetAllLODActorsInLevel();
	void CreateNewClusterFromActors(TArray<TObjectPtr<AActor>> InActors, int32 InLODLevel = 0);
	bool GenerateShadowproxyFromLODActors();
	void ExclusiveSelect();
}

UCLASS()
class UPineappleCore : public UObject
{
	GENERATED_BODY()
	
};
