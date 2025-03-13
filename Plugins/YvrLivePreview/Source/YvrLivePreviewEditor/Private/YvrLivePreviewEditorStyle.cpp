// Copyright 2020-2024 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrLivePreviewEditorStyle.h"
#include "YvrLivePreviewEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FYvrLivePreviewEditorStyle::StyleInstance = NULL;

void FYvrLivePreviewEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FYvrLivePreviewEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FYvrLivePreviewEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("YvrLivePreviewEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FYvrLivePreviewEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("YvrLivePreviewEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("YvrLivePreview")->GetBaseDir() / TEXT("Resources"));

	Style->Set("YvrLivePreviewEditor.PluginAction", new IMAGE_BRUSH(TEXT("Icon128"), Icon40x40));
	return Style;
}

#undef IMAGE_BRUSH

void FYvrLivePreviewEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FYvrLivePreviewEditorStyle::Get()
{
	return *StyleInstance;
}
