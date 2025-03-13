#include "OnlineSubsystemYvrModule.h"

#include "Misc/ConfigCacheIni.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemModule.h"
#include "OnlineSubsystemYvr.h"
#include "OnlineSettingsYvr.h"

#if WITH_EDITOR
#include "PropertyEditorModule.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "OnlineSettingsCustomizationYvr.h"
#endif

#define LOCTEXT_NAMESPACE "FOnlineYvrModule"

IMPLEMENT_MODULE(FOnlineSubsystemYvrModule, OnlineSubsystemYvr);

/**
 * Class responsible for creating instance(s) of the subsystem
 */
class FOnlineFactoryYvr : public IOnlineFactory
{
private:

	/** Singleton instance of the Yvr OnlineSubsystem */
	FOnlineSubsystemYvrPtr OnlineSub;

public:

	FOnlineFactoryYvr() {}
	virtual ~FOnlineFactoryYvr() {}

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName) override
	{
		UE_LOG_ONLINE(Warning, TEXT("Yvr CreateSubsystem"));
		if (!OnlineSub.IsValid())
		{
			OnlineSub = MakeShared<FOnlineSubsystemYvr, ESPMode::ThreadSafe>(InstanceName);
		}
		if (!OnlineSub->IsInitialized())
		{
			if (!OnlineSub->Init())
			{
				UE_LOG_ONLINE(Warning, TEXT("Yvr API failed to initialize!"));
				// Shutdown already called in Init() when this failed
				OnlineSub = nullptr;
			}
		}
		else
		{
			UE_LOG_ONLINE(Log, TEXT("Yvr API already initialized!"));
		}

		return OnlineSub;
	}
};

void FOnlineSubsystemYvrModule::StartupModule()
{
	UE_LOG_ONLINE(Log, TEXT("Yvr Startup!"));
	YvrFactory = new FOnlineFactoryYvr();
	// Create and register our singleton factory with the main online subsystem for easy access
	FOnlineSubsystemModule& OnlineSubsystemModule = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OnlineSubsystemModule.RegisterPlatformService(YVR_SUBSYSTEM, YvrFactory);

#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(
		UOnlineSettingsYvr::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FOnlineSettingsCustomizationYvr::MakeInstance)
	);
	PropertyModule.NotifyCustomizationModuleChanged();

	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "OnlineSettingsYvr",
			LOCTEXT("OnlineSettingsYvrName", "YVR OnlineSettings"),
			LOCTEXT("OnlineSettingsYvrDescription", "Configure the OnlineSubsystemYvr plug-in."),
			GetMutableDefault<UOnlineSettingsYvr>()
		);
	}
#endif
}

void FOnlineSubsystemYvrModule::ShutdownModule()
{
	UE_LOG_ONLINE(Log, TEXT("Yvr Shutdown!"));

#if WITH_EDITOR
	// unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "OnlineSettingsYvr");
	}
#endif

	FOnlineSubsystemModule& OnlineSubsystemModule = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OnlineSubsystemModule.UnregisterPlatformService(YVR_SUBSYSTEM);

	delete YvrFactory;
	YvrFactory = nullptr;
}
