// Fill out your copyright notice in the Description page of Project Settings.


#include "SMainWidget.h"
#include "SlateOptMacros.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/LODActor.h"
#include "Engine/HLODProxy.h"
#include "Editor/GroupActor.h"
#include "IHierarchicalLODUtilities.h"
#include "HierarchicalLODUtilitiesModule.h"
#include "EngineUtils.h"
#include "Selection.h"


#define LOCTEXT_NAMESPACE "SMainWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SMainWidget::Construct(const FArguments& InArgs)
{	
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot().AutoWidth()/*.HAlign(HAlign_Left)*/.VAlign(VAlign_Top)
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
		+ SHorizontalBox::Slot().HAlign(HAlign_Right).VAlign(VAlign_Fill)
		[	
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).OnClicked(this, &SMainWidget::btn_refreshhlodstatus_onclicked).Text(LOCTEXT("btn_updatehlodstatus", "Refresh"))
				]
			]
			+ SVerticalBox::Slot().VAlign(VAlign_Fill)
			[
				SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SBox).MinDesiredWidth(500.0f).MaxDesiredWidth(500.0f)
					[
						SAssignNew(HLODProxyListView, SListView< FHLODProxyListItemType >)
						.ListItemsSource(&HLODProxyListItems)
						.OnGenerateRow(this, &SMainWidget::HLODProxyList_OnGenerateRow)
						.SelectionMode(ESelectionMode::None)
						.HeaderRow
						(
							SNew(SHeaderRow)
							+ SHeaderRow::Column(FName(TEXT("LABEL")))
							.DefaultLabel(LOCTEXT("LABEL", "LABEL"))
						)
					]
				]
			]	
		]
	];

#if 0
	//HLODProxyListView test
	HLODProxyListItems.Add( MakeShared<FHLODProxyListItem>(FHLODProxyListItem(TEXT("test1111"))) );
	HLODProxyListItems.Add( MakeShared<FHLODProxyListItem>(FHLODProxyListItem(TEXT("test222"))) );
	HLODProxyListItems.Add( MakeShared<FHLODProxyListItem>(FHLODProxyListItem(TEXT("test23333"))) );
	HLODProxyListView->RequestListRefresh();
#endif
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

FReply SMainWidget::btn_refreshhlodstatus_onclicked()
{
	HLODProxyListItems.Empty();

	TArray<TObjectPtr<ALODActor>> LODActors = Pineapple::GetAllLODActorsInLevel();
	if (LODActors.Num() > 0)
	{
		for (int32 i = 0; i < LODActors.Num(); i++)
		{
			HLODProxyListItems.Add(MakeShared<FHLODProxyListItem>(FHLODProxyListItem(LODActors[i])));
		}	
	}

	HLODProxyListView->RequestListRefresh();

	return FReply::Handled();
}

TSharedRef<ITableRow> SMainWidget::HLODProxyList_OnGenerateRow(FHLODProxyListItemType InItem, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(SHLODProxyListRow, OwnerTable)
		.LODActor(InItem->LODActor);
}

void SHLODProxyListRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	LODActorPtr = InArgs._LODActor;

	SMultiColumnTableRow<TSharedPtr<int32> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SHLODProxyListRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	//if (ColumnName == TEXT("LABEL"))
	
	if (LODActorPtr.IsValid())
	{
		const bool bIsBuilt = LODActorPtr->IsBuilt();

		TObjectPtr<UHLODProxy> HLODProxy = LODActorPtr->GetProxy();

		//proxy mesh
		const bool bValidProxyMesh = !(LODActorPtr->GetStaticMeshComponent()->GetStaticMesh() && HLODProxy.IsNull());

		//Key	
		const bool bMismatchKey = HLODProxy ? HLODProxy->ContainsDataForActor(LODActorPtr.Get()) : false;	

		//subactor link
		auto GetSubActorLinkValid = [](TWeakObjectPtr<ALODActor> InLODActor) ->bool {
			for (auto SubActor = InLODActor->SubActors.CreateIterator(); SubActor; ++SubActor)
			{
				if (*SubActor)
				{
					UStaticMeshComponent* SMComponent = (*SubActor)->FindComponentByClass<UStaticMeshComponent>();
					UStaticMeshComponent* LODComponent = SMComponent ? Cast<UStaticMeshComponent>(SMComponent->GetLODParentPrimitive()) : nullptr;
					if (LODComponent == nullptr || LODComponent->GetOwner() != InLODActor || LODComponent->GetStaticMesh() == nullptr)
					{
						return false;
					}
				}
			}
			return true;
		};
		const bool bSubActorLinkValid = GetSubActorLinkValid(LODActorPtr);

		// Unbuilt children
		auto GetChildrenBuilt = [](TWeakObjectPtr<ALODActor> InLODActor) ->bool {
			for (auto SubActor = InLODActor->SubActors.CreateIterator(); SubActor; ++SubActor)
			{
				if (ALODActor* SubLODActor = Cast<ALODActor>(*SubActor))
				{
					if (!SubLODActor->IsBuilt(true))
					{
						return false;
					}
				}
			}
			return true;
		};
		const bool bChildrenBuilt = GetChildrenBuilt(LODActorPtr);

		FText LabelText = FText::FromString(LODActorPtr->GetActorLabel());
		FText IsBuildText = FText::FromString(bIsBuilt ? TEXT("[Built]") : TEXT("[unBuilt]"));
		FText bValidProxyMeshText = FText::FromString(bMismatchKey ? TEXT("ProxyMesh valid") : TEXT("ProxyMesh invalid"));
		FText bMismatchKeyText = FText::FromString(bMismatchKey ? TEXT("Key valid") : TEXT("Key invalid"));
		FText bSubActorLinkValidText = FText::FromString(bMismatchKey ? TEXT("SubActorLink valid") : TEXT("SubActorLink invalid"));
		FText bChildrenBuiltText = FText::FromString(bMismatchKey ? TEXT("Children built") : TEXT("Children unbuilt"));

		return SNew(SBox).Padding(FMargin(2.0f))
			[
				SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot().Padding(2.0f, 0.0f).AutoWidth()
						[
							SNew(STextBlock).Text(LabelText)
						]						
						+SHorizontalBox::Slot().Padding(2.0f, 0.0f).AutoWidth()
						[
							SNew(STextBlock).Text(IsBuildText)
						]
					]
					+ SVerticalBox::Slot()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().Padding(2.0f, 0.0f)
						[
							SNew(STextBlock).Text(bValidProxyMeshText)
						]
						+ SHorizontalBox::Slot().Padding(2.0f, 0.0f)
						[
							SNew(STextBlock).Text(bMismatchKeyText)
						]
						+ SHorizontalBox::Slot().Padding(2.0f, 0.0f)
						[
							SNew(STextBlock).Text(bSubActorLinkValidText)
						]
						+ SHorizontalBox::Slot().Padding(2.0f, 0.0f)
						[
							SNew(STextBlock).Text(bChildrenBuiltText)
						]
					]
				]
			];
	}

	return SNew(SBox)
		.Padding(FMargin(4.0, 0.0))
		[
			SNew(STextBlock).Text(LOCTEXT("INVALID", "INVALID"))
		];
}

#undef LOCTEXT_NAMESPACE