// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "YvrLivePreviewEditorStyle.h"

class FYvrLivePreviewEditorCommands : public TCommands<FYvrLivePreviewEditorCommands>
{
public:

	FYvrLivePreviewEditorCommands()
		: TCommands<FYvrLivePreviewEditorCommands>(TEXT("YvrLivePreviewEditor"), NSLOCTEXT("Contexts", "YvrLivePreviewEditor", "YvrLivePreview Plugin"), NAME_None, FYvrLivePreviewEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
