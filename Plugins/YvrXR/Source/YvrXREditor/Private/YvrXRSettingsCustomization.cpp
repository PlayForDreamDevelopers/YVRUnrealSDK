// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXRSettingsCustomization.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "YvrRuntimeSettings"

FYvrXRSettingsCustomization::FYvrXRSettingsCustomization()
    :SavedLayoutBuilder(nullptr)
{
    YvrSettings = GetMutableDefault<UYvrXRHMDSettings>();
}

TSharedRef<IDetailCustomization> FYvrXRSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FYvrXRSettingsCustomization);
}

void FYvrXRSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    SavedLayoutBuilder = &DetailLayout;
    DetailLayout.EditCategory(TEXT("YvrMobile"));
}
#endif