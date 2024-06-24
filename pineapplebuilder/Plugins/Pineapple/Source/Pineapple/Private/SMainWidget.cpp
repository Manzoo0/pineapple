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
			+SVerticalBox::Slot().AutoHeight()
			[
				SNew(SButton).OnClicked(this, &SMainWidget::btn_generateshadowproxyfromlodactor_onclicked)	[ SNew(STextBlock).Text(LOCTEXT("btn_generateshadowproxy", "Generate Shadow Proxy")) ]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SMainWidget::btn_tohlodproxy_onclicked()
{
	Pineapple::CreateNewClusterFromActors(Pineapple::GetSelectedActors());
	return FReply::Handled();
}

FReply SMainWidget::btn_exclusiveselect_onclicked()
{
	Pineapple::ExclusiveSelect();
	return FReply::Handled();
}

FReply SMainWidget::btn_generateshadowproxyfromlodactor_onclicked()
{
	return Pineapple::GenerateShadowproxyFromLODActors() ? FReply::Handled() : FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE