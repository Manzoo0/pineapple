// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PineappleCore.h"

#include "Widgets/Views/SListView.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#include "Widgets/SCompoundWidget.h"

struct FHLODProxyListItem
{
	TWeakObjectPtr<ALODActor> LODActor;

	FHLODProxyListItem() {}
	FHLODProxyListItem(TObjectPtr<ALODActor> inLODActor)
		: LODActor(inLODActor)
	{}
};

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
	FReply btn_generateshadowproxyfromlodactor_onclicked();
	FReply btn_refreshhlodstatus_onclicked();

private:
	using FHLODProxyListItemType = TSharedPtr <FHLODProxyListItem>;

	TArray<FHLODProxyListItemType> HLODProxyListItems;
	TSharedPtr< SListView< FHLODProxyListItemType > > HLODProxyListView;
	TSharedRef<ITableRow> HLODProxyList_OnGenerateRow(FHLODProxyListItemType InItem, const TSharedRef<STableViewBase>& OwnerTable) const;
};

class SHLODProxyListRow : public SMultiColumnTableRow<TSharedPtr<int32>>
{
	SLATE_BEGIN_ARGS(SHLODProxyListRow) {}
		SLATE_ARGUMENT(TWeakObjectPtr<ALODActor>, LODActor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TWeakObjectPtr<ALODActor> LODActorPtr;
};