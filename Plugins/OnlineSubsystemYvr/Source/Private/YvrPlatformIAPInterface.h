#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemYvr.h"

DECLARE_LOG_CATEGORY_EXTERN(YvrIAP, Log, All);

DECLARE_DELEGATE_OneParam(FGetProductsBySKUDelegate, const bool);
DECLARE_DELEGATE_OneParam(FGetViewerPurchasesDelegate, const bool);
DECLARE_DELEGATE_OneParam(FConsumePurchaseDelegate, const bool);
DECLARE_DELEGATE_OneParam(FLaunchCheckoutFlowDelegate, const bool);


enum class EProductType : uint8
{
	Invalid,
	Consumable,
	Durable,
	Unknown
};

struct FYvrPurchase
{
	FString Id;
	FString Sku;
	FString Name;
	EProductType Type;
	FString Icon;
};

struct FYvrProduct
{
	FString Sku;
	FString Name;
	EProductType Type;
	FString Icon;
	FString Description;
	float Price;
};

class ONLINESUBSYSTEMYVR_API FYvrPlatformIAPInterface
{
private:

    FOnlineSubsystemYvr& YvrSubsystem;

	FYvrPurchase CurrentPurchase{};
	TArray<FYvrProduct> CachedProducts;
	TArray<FYvrPurchase> CachedPurchases;

public:
	FYvrPlatformIAPInterface(FOnlineSubsystemYvr& InSubsystem);
    ~FYvrPlatformIAPInterface();

	void ConsumePurchase(const FString& Sku, const FConsumePurchaseDelegate& Delegate = FConsumePurchaseDelegate());
	void GetProductsBySku(const TArray<FString>& ProductSkus, const FGetProductsBySKUDelegate& Delegate = FGetProductsBySKUDelegate());
	void GetViewerPurchases(const FGetViewerPurchasesDelegate& Delegate = FGetViewerPurchasesDelegate());
	void LaunchCheckoutFlow(const FString& Sku, const float& Price, const FLaunchCheckoutFlowDelegate& Delegate = FLaunchCheckoutFlowDelegate());

	void GetCachedProducts(TArray<FYvrProduct>& OutProducts);
	void GetCachedPurchases(TArray<FYvrPurchase>& OutPurchases);
	void GetCurrentPurchase(FYvrPurchase& OutPurchase);
};