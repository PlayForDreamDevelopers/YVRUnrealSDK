// Copyright 2020-2023 Yvr Technology Co., Ltd. All Rights Reserved.

#include "YvrXREditor.h"
#include "YvrXRSettingsCustomization.h"

#include "ISettingsModule.h"

static const FName YvrXREditorTabName("YvrXREditor");

#define LOCTEXT_NAMESPACE "FYvrXREditorModule"

void FYvrXREditorModule::StartupModule()
{
	RegisterSettings();
}

void FYvrXREditorModule::ShutdownModule()
{

}

void FYvrXREditorModule::RegisterSettings()
{

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		UYvrXRHMDSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FYvrXRSettingsCustomization::MakeInstance)
	);
	PropertyModule.NotifyCustomizationModuleChanged();

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "YvrXR",
			LOCTEXT("YvrXRSettingsName", "YVR XR"),
			LOCTEXT("YvrXRSettingsDescription", "Configure the YvrXR plug-in."),
			GetMutableDefault<UYvrXRHMDSettings>()
		);
	}
}

void FYvrXREditorModule::UnregisterSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "YvrXR");
	}
}

IMPLEMENT_MODULE(FYvrXREditorModule, YvrXREditor)

#undef LOCTEXT_NAMESPACE