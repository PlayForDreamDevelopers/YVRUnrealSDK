// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrLivePreviewEditorCommands.h"

#define LOCTEXT_NAMESPACE "FYvrLivePreviewEditorModule"

void FYvrLivePreviewEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Test", "Execute YvrLivePreviewEditor action", EUserInterfaceActionType::Button, FInputGesture());
}


#undef LOCTEXT_NAMESPACE
