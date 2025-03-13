// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.
#include "OnlineSettingsCustomizationYvr.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "YvrRuntimeSettings"

FOnlineSettingsCustomizationYvr::FOnlineSettingsCustomizationYvr()
    :SavedLayoutBuilder(nullptr)
{
    YvrSettings = GetMutableDefault<UOnlineSettingsYvr>();
}

TSharedRef<IDetailCustomization> FOnlineSettingsCustomizationYvr::MakeInstance()
{
    return MakeShareable(new FOnlineSettingsCustomizationYvr);
}

void FOnlineSettingsCustomizationYvr::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
    SavedLayoutBuilder = &DetailLayout;
    DetailLayout.EditCategory(TEXT("YvrMobile"));
}
#endif