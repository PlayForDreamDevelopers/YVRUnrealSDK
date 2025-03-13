// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrLivePreviewEditor.h"
#include "YvrLivePreviewEditorStyle.h"
#include "YvrLivePreviewEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName YvrLivePreviewEditorTabName("YvrLivePreviewEditor");

#define LOCTEXT_NAMESPACE "FYvrLivePreviewEditorModule"

void FYvrLivePreviewEditorModule::StartupModule()
{
	FYvrLivePreviewEditorStyle::Initialize();
	FYvrLivePreviewEditorStyle::ReloadTextures();

	FYvrLivePreviewEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FYvrLivePreviewEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FYvrLivePreviewEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FYvrLivePreviewEditorModule::RegisterMenus));
}

void FYvrLivePreviewEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FYvrLivePreviewEditorStyle::Shutdown();

	FYvrLivePreviewEditorCommands::Unregister();
}

void FYvrLivePreviewEditorModule::PluginButtonClicked()
{
	if (YvrPreviewManager::GetInstance()->GetServiceState() == PREVIEW_STOP)
	{
		YvrPreviewManager::GetInstance()->Start();
	}
	else
	{
		YvrPreviewManager::GetInstance()->Stop();
	}
}

void FYvrLivePreviewEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");

	FToolMenuEntry& Entry = Section.AddEntry(
		FToolMenuEntry::InitToolBarButton(
			FYvrLivePreviewEditorCommands::Get().PluginAction,
			TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FYvrLivePreviewEditorModule::GetText)),
			TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FYvrLivePreviewEditorModule::GetTips)),
			TAttribute<FSlateIcon>::Create(TAttribute<FSlateIcon>::FGetter::CreateRaw(this, &FYvrLivePreviewEditorModule::GetImage))
		));
	Entry.SetCommandList(PluginCommands);
}

// set button state
FSlateIcon FYvrLivePreviewEditorModule::GetImage()
{
	switch (YvrPreviewManager::GetInstance()->GetServiceState())
	{
	case PREVIEW_START:
		return FSlateIcon(FYvrLivePreviewEditorStyle::GetStyleSetName(), "YvrLivePreviewEditor.PluginAction");
	case PREVIEW_CONNECTED:
		return FSlateIcon(FYvrLivePreviewEditorStyle::GetStyleSetName(), "YvrLivePreviewEditor.PluginAction");
	case PREVIEW_STOP:
		return FSlateIcon(FYvrLivePreviewEditorStyle::GetStyleSetName(), "YvrLivePreviewEditor.PluginAction");
	default:
		return FSlateIcon(FYvrLivePreviewEditorStyle::GetStyleSetName(), "YvrLivePreviewEditor.PluginAction");
	}
}

FText FYvrLivePreviewEditorModule::GetText()
{
	switch (YvrPreviewManager::GetInstance()->GetServiceState())
	{
	case PREVIEW_START:
		return FText::FromString("preview start");
	case PREVIEW_CONNECTED:
		return FText::FromString("preview connect");
	case PREVIEW_STOP:
		return FText::FromString("preview stop");
	default:
		return FText::FromString("default");
	}
}

FText FYvrLivePreviewEditorModule::GetTips()
{
	switch (YvrPreviewManager::GetInstance()->GetServiceState())
	{
	case PREVIEW_START:
		return FText::FromString("click to stop preview");
	case PREVIEW_CONNECTED:
		return FText::FromString("click to stop preview");
	case PREVIEW_STOP:
		return FText::FromString("click to start preview");
	default:
		return FText::FromString("default");
	}
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FYvrLivePreviewEditorModule, YvrLivePreviewEditor)