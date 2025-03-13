// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YvrPreviewManager.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FYvrLivePreviewEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command. */
	void PluginButtonClicked();

	// set image and text by connect state
	FSlateIcon GetImage();
	FText GetText();
	FText GetTips();


private:
	void RegisterMenus();
	bool isConnect;


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
