// The MIT License (MIT)

// Copyright 2020 HankShu inkiu0@gmail.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ELuaMonitorPanel.h"
#include "ELuaMonitor.h"
#include "EditorStyleSet.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SNumericDropDown.h"

SELuaMonitorPanel::SELuaMonitorPanel()
{

}

SELuaMonitorPanel::~SELuaMonitorPanel()
{

}

TSharedRef<class SDockTab> SELuaMonitorPanel::GetSDockTab()
{
	UpdateRoot();


	TArray<SNumericDropDown<float>::FNamedValue> NamedValuesForMonitorMode;
	NamedValuesForMonitorMode.Add(SNumericDropDown<float>::FNamedValue((float)ELuaMonitorMode::PerFrame, FText::FromName("PerFrame"), FText::FromName("PerFrameRecord")));
	NamedValuesForMonitorMode.Add(SNumericDropDown<float>::FNamedValue((float)ELuaMonitorMode::Total, FText::FromName("Total"), FText::FromName("RecordTotalInfo")));
	NamedValuesForMonitorMode.Add(SNumericDropDown<float>::FNamedValue((float)ELuaMonitorMode::Statistics, FText::FromName("Statistics"), FText::FromName("Tile all node")));


	// Init TreeViewWidget
	SAssignNew(TreeViewWidget, STreeView<TSharedPtr<FELuaTraceInfoNode>>)
		.ItemHeight(800)
		.TreeItemsSource(&ShowRootList)
		.OnGenerateRow_Raw(this, &SELuaMonitorPanel::OnGenerateRow)
		.OnGetChildren_Raw(this, &SELuaMonitorPanel::OnGetChildrenRaw)
		.SelectionMode(ESelectionMode::None)
		.HeaderRow
		(
			SNew(SHeaderRow)
			+ SHeaderRow::Column("Name").DefaultLabel(FText::FromName("Name")).HAlignHeader(HAlign_Fill)
			+ SHeaderRow::Column("TotalTime(ms)").DefaultLabel(FText::FromName("TotalTime(ms)")).FixedWidth(COL_WIDTH)
			+ SHeaderRow::Column("TotalTime(%)").DefaultLabel(FText::FromName("TotalTime(%)")).FixedWidth(30)
			+ SHeaderRow::Column("SelfTime(ms)").DefaultLabel(FText::FromName("SelfTime(ms)")).FixedWidth(COL_WIDTH)
			+ SHeaderRow::Column("SelfTime(%)").DefaultLabel(FText::FromName("SelfTime(%)")).FixedWidth(30)
			+ SHeaderRow::Column("Average(ms)").DefaultLabel(FText::FromName("Average(ms)")).FixedWidth(COL_WIDTH)
			+ SHeaderRow::Column("Alloc(kb)").DefaultLabel(FText::FromName("Alloc(kb)")).FixedWidth(COL_WIDTH)
			+ SHeaderRow::Column("Alloc(%)").DefaultLabel(FText::FromName("Alloc(%)")).FixedWidth(30)
			+ SHeaderRow::Column("GC(kb)").DefaultLabel(FText::FromName("GC(kb)")).FixedWidth(COL_WIDTH)
			+ SHeaderRow::Column("GC(%)").DefaultLabel(FText::FromName("GC(%)")).FixedWidth(30)
			+ SHeaderRow::Column("Calls").DefaultLabel(FText::FromName("Calls")).FixedWidth(COL_WIDTH)
		);


	return SNew(SDockTab)
	.Icon(FEditorStyle::GetBrush("Kismet.Tabs.Palette"))
	.Label(FText::FromName("ELuaMonitor"))
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().MaxHeight(30)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			//.BorderBackgroundColor(FLinearColor(.50f, .50f, .50f, 1.0f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Center).AutoWidth()
				[
					SNew(SNumericDropDown<float>)
					.LabelText(FText::FromName("Mode:"))
					.bShowNamedValue(true)
					.DropDownValues(NamedValuesForMonitorMode)
					.IsEnabled(true)
					.Value_Lambda([this]() { return (float)MonitorMode; })
					.OnValueChanged_Raw(this, &SELuaMonitorPanel::OnModeChanged)
				]

				+SHorizontalBox::Slot()
				[
					SNew(SSpacer)
				]

				+ SHorizontalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center).AutoWidth()
				[
					SAssignNew(PrevFrameBtn, SButton)
					.ButtonStyle(FEditorStyle::Get(), "NoBorder")																// �޵�ͼ���ⰴť����
					.ContentPadding(2.0)
					.IsFocusable(false)
					//.OnClicked_Raw(this, &SELuaMonitorPanel::OnForwardBtnClicked)
					[
						SNew(SImage)
						.Image_Raw(this, &SELuaMonitorPanel::GetPrevFrameIcon)
					]
				]
				+ SHorizontalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center).AutoWidth()
				[
					SAssignNew(ForwardBtn, SButton)
					.ButtonStyle(FEditorStyle::Get(), "NoBorder")																// �޵�ͼ���ⰴť����
					.ContentPadding(2.0)
					.IsFocusable(false)
					.OnClicked_Raw(this, &SELuaMonitorPanel::OnForwardBtnClicked)
					[
						SNew(SImage)
						.Image_Raw(this, &SELuaMonitorPanel::GetForwardIcon)
					]
				]
				+ SHorizontalBox::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center).AutoWidth()
				[
					SAssignNew(NextFrameBtn, SButton)
					.ButtonStyle(FEditorStyle::Get(), "NoBorder")																// �޵�ͼ���ⰴť����
					.ContentPadding(2.0)
					.IsFocusable(false)
					//.OnClicked_Raw(this, &SELuaMonitorPanel::OnForwardBtnClicked)
					[
						SNew(SImage)
						.Image_Raw(this, &SELuaMonitorPanel::GetNextFrameIcon)
					]
				]

				+SHorizontalBox::Slot()
				[
					SNew(SSpacer)
				]

				+ SHorizontalBox::Slot().HAlign(HAlign_Right).VAlign(VAlign_Center).AutoWidth()
				[
					SNew(SButton)
					.ButtonStyle(FEditorStyle::Get(), "NoBorder")																// �޵�ͼ���ⰴť����
					.ContentPadding(2.0)
					.IsFocusable(false)
					.OnClicked_Raw(this, &SELuaMonitorPanel::OnClearBtnClicked)
					.ToolTipText(FText::FromName("Clear and stop monitor"))
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush("Cross"))
					]
				]
			]
		]

		+ SVerticalBox::Slot()
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				TreeViewWidget.ToSharedRef()
			]
		]
	];
}

TSharedRef<ITableRow> SELuaMonitorPanel::OnGenerateRow(TSharedPtr<FELuaTraceInfoNode> TINode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
	SNew(STableRow<TSharedPtr<FELuaTraceInfoNode>>, OwnerTable)
	/*.Padding(2.0f).Visibility_Lambda([=]() {
		return EVisibility::Visible;
	})*/
	[
		SNew(SHeaderRow)
		+ SHeaderRow::Column("Name").DefaultLabel(FText::FromString(TINode->ID))
		.DefaultTooltip(FText::FromString(TINode->ID)).HAlignHeader(HAlign_Fill)
		//+ SHeaderRow::Column("Global TotalTime(%)").DefaultLabel(TAttribute<FText>::Create([=]() {
		//	return FText::AsNumber(TINode->TotalTime / CurRootTINode->TotalTime);
		//}))
		//+ SHeaderRow::Column("Global SelfTime(%)").DefaultLabel(TAttribute<FText>::Create([=]() {
		//	return FText::AsNumber(TINode->SelfTime / CurRootTINode->SelfTime);
		//}))
		+SHeaderRow::Column("TotalTime(ms)").FixedWidth(COL_WIDTH).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->TotalTime / 1000000.f);
		}))
		+ SHeaderRow::Column("TotalTime(%)").FixedWidth(30).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(CurRootTINode->TotalTime > 0 ? TINode->TotalTime / CurRootTINode->TotalTime : 0);
		}))
		+ SHeaderRow::Column("SelfTime(ms)").FixedWidth(COL_WIDTH).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->SelfTime / 1000000.f);
		}))
		+ SHeaderRow::Column("SelfTime(%)").FixedWidth(30).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(CurRootTINode->SelfTime > 0 ? TINode->SelfTime / CurRootTINode->SelfTime : 0);
		}))
		+ SHeaderRow::Column("Average(ms)").FixedWidth(COL_WIDTH).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->Count > 0 ? TINode->TotalTime / (TINode->Count * 1000000.f) : 0);
		}))
		+ SHeaderRow::Column("Alloc(kb)").FixedWidth(COL_WIDTH).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->AllocSize / 1000000.f);
		}))
		+ SHeaderRow::Column("Alloc(%)").FixedWidth(30).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(CurRootTINode->AllocSize > 0 ? TINode->AllocSize / CurRootTINode->AllocSize : 0);
		}))
		+ SHeaderRow::Column("GC(kb)").FixedWidth(COL_WIDTH).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->GCSize / 1000000.f);
		}))
		+ SHeaderRow::Column("GC(%)").FixedWidth(30).DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(CurRootTINode->GCSize > 0 ? TINode->GCSize / CurRootTINode->GCSize : 0);
		}))
		+ SHeaderRow::Column("Calls").DefaultLabel(TAttribute<FText>::Create([=]() {
			return FText::AsNumber(TINode->Count);
		}))
		.FixedWidth(COL_WIDTH)
	];
}

void SELuaMonitorPanel::OnGetChildrenRaw(TSharedPtr<FELuaTraceInfoNode> TINode, TArray<TSharedPtr<FELuaTraceInfoNode>>& OutChildren)
{
	if (TINode)
	{
		OutChildren = TINode->Children;
	}
}

void SELuaMonitorPanel::UpdateRoot()
{
	switch (MonitorMode)
	{
	case PerFrame:
		break;
	case Statistics:
		break;
	case Total:
	default:
		CurRootTINode = FELuaMonitor::GetInstance()->GetRoot();
		if (CurRootTINode)
		{
			ShowRootList = CurRootTINode->Children;
		}
		else
		{
			ShowRootList = {};
		}
		break;
	}
}

void SELuaMonitorPanel::Tick(float DeltaTime)
{
	FELuaMonitor::GetInstance()->Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	if (ElapsedTime > UPDATE_INTERVAL)
	{
		UpdateRoot();

		if (TreeViewWidget.IsValid())
		{
			//TreeViewWidget->RebuildList();

			TreeViewWidget->RequestTreeRefresh();
		}

		ElapsedTime = 0.f;
	}
}

FReply SELuaMonitorPanel::OnForwardBtnClicked()
{
	FELuaMonitor::GetInstance()->OnForward();
	return FReply::Handled();
}

FReply SELuaMonitorPanel::OnClearBtnClicked()
{
	FELuaMonitor::GetInstance()->OnClear();
	return FReply::Handled();
}

const FSlateBrush* SELuaMonitorPanel::GetPrevFrameIcon() const
{
	return  &FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Backward_Step").Normal;
}

const FSlateBrush* SELuaMonitorPanel::GetForwardIcon() const
{
	if (FELuaMonitor::GetInstance()->IsRuning())
	{
		return &FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Pause").Normal;
	}
	return  &FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward").Normal;
}

const FSlateBrush* SELuaMonitorPanel::GetNextFrameIcon() const
{
	return  &FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("Animation.Forward_Step").Normal;
}

void SELuaMonitorPanel::OnModeChanged(float InMode)
{
	if (InMode > -1 && InMode < ((uint8)ELuaMonitorMode::MAX))
	{
		if (!FMath::IsNearlyEqual((float)MonitorMode, InMode))
		{
			MonitorMode = (ELuaMonitorMode)InMode;
		}
	}
}

void SELuaMonitorPanel::OnDestroy()
{
	FELuaMonitor::GetInstance()->OnClear();
}
