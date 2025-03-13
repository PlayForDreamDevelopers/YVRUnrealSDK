// Copyright 2020-2021 Yvr Technology Co., Ltd. All Rights Reserved.
#pragma once
#if WITH_EDITOR
#include "IDetailCustomization.h"
#include "OnlineSettingsYvr.h"

class FOnlineSettingsCustomizationYvr : public IDetailCustomization
{
public:
    // Makes a new instance of this detail layout class for a specific detail view requesting it
    static TSharedRef<IDetailCustomization> MakeInstance();

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface
    FOnlineSettingsCustomizationYvr();
private:
    IDetailLayoutBuilder* SavedLayoutBuilder;
    UOnlineSettingsYvr* YvrSettings;
};
#endif