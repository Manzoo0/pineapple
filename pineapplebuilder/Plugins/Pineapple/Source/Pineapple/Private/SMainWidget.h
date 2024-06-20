// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class SMainWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMainWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FReply btn_tohlodproxy_onclicked();
	FReply btn_exclusiveselect_onclicked();

private:
	void SetupHLODClusterForEachGroup(TArray<AActor*> InActors, int32 InLODLevel = 0);
};
