#include "YvrPlatformIAPInterface.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemYvrWrapper.h"

DEFINE_LOG_CATEGORY(YvrIAP);

void FYvrPlatformIAPInterface::ConsumePurchase(const FString& Sku, const FConsumePurchaseDelegate& Delegate)
{
#if PLATFORM_ANDROID
    YvrSubsystem.AddRequestDelegate(
        OnlineSubsystemYvrWrapper::ConsumePurchase(Sku),
        FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
        {
            if (bIsError)
            {
                auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
                UE_LOG(YvrIAP, Log, TEXT("ConsumePurchase failed: %s"), *ErrorMessage);
                Delegate.ExecuteIfBound(false);
            }
            else
            {
                UE_LOG(YvrIAP, Log, TEXT("ConsumePurchase Successfully"));
                Delegate.ExecuteIfBound(true);
            }
        }));
#endif
}

void FYvrPlatformIAPInterface::GetProductsBySku(const TArray<FString>& ProductSkus, const FGetProductsBySKUDelegate& Delegate)
{
#if PLATFORM_ANDROID
    YvrSubsystem.AddRequestDelegate(
        OnlineSubsystemYvrWrapper::GetProductsBySKU(ProductSkus),
        FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
        {
            CachedProducts.Empty();
            if (bIsError)
            {
                auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
                UE_LOG(YvrIAP, Log, TEXT("GetProductsBySku failed: %s"), *ErrorMessage);
                Delegate.ExecuteIfBound(false);
            }
            else
            {
                int ProductSize = OnlineSubsystemYvrWrapper::GetProductSize(Message);
                for (int i = 0; i < ProductSize; ++i)
                {
                    auto Product = OnlineSubsystemYvrWrapper::GetProductByIndex(Message, i);
                    FYvrProduct NewProduct{};
                    NewProduct.Sku = OnlineSubsystemYvrWrapper::GetSkuOfProduct(Product);
                    NewProduct.Name = OnlineSubsystemYvrWrapper::GetNameOfProduct(Product);
                    NewProduct.Price = OnlineSubsystemYvrWrapper::GetPriceOfProduct(Product);
                    NewProduct.Type = (EProductType)OnlineSubsystemYvrWrapper::GetTypeOfProduct(Product);
                    NewProduct.Icon = OnlineSubsystemYvrWrapper::GetScoverOfProduct(Product);
                    NewProduct.Description = OnlineSubsystemYvrWrapper::GetBriefOfProduct(Product);
                    CachedProducts.Add(NewProduct);
                }
                Delegate.ExecuteIfBound(true);
            }
        }));
#endif
}

void FYvrPlatformIAPInterface::GetViewerPurchases(const FGetViewerPurchasesDelegate& Delegate)
{
#if PLATFORM_ANDROID
    YvrSubsystem.AddRequestDelegate(
        OnlineSubsystemYvrWrapper::GetViewerPurchases(),
        FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
        {
            CachedPurchases.Empty();
            if (bIsError)
            {
                auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
                UE_LOG(YvrIAP, Log, TEXT("GetViewerPurchases failed:%s"), *ErrorMessage);
                Delegate.ExecuteIfBound(false);
            }
            else
            {
                int PurchaseSize = OnlineSubsystemYvrWrapper::GetPurchaseSize(Message);
                for (int i = 0; i < PurchaseSize; ++i)
                {
                    auto Purchase = OnlineSubsystemYvrWrapper::GetPurchaseByIndex(Message, i);

                    FYvrPurchase NewPurchase{};
                    NewPurchase.Sku = OnlineSubsystemYvrWrapper::GetSkuOfPurchase(Purchase);
                    NewPurchase.Name = OnlineSubsystemYvrWrapper::GetNameOfPurchase(Purchase);
                    NewPurchase.Id = OnlineSubsystemYvrWrapper::GetTradeNoOfPurchase(Purchase);
                    NewPurchase.Type = (EProductType)OnlineSubsystemYvrWrapper::GetTypeOfPurchase(Purchase);
                    NewPurchase.Icon = OnlineSubsystemYvrWrapper::GetScoverOfPurchase(Purchase);
                    CachedPurchases.Add(NewPurchase);
                }
                Delegate.ExecuteIfBound(true);
            }
        }));
#endif
}

void FYvrPlatformIAPInterface::LaunchCheckoutFlow(const FString& Sku, const float& Price, const FLaunchCheckoutFlowDelegate& Delegate)
{
#if PLATFORM_ANDROID
    YvrSubsystem.AddRequestDelegate(
        OnlineSubsystemYvrWrapper::LaunchCheckoutFlow(Sku, Price),
        FYvrMessageOnCompleteDelegate::CreateLambda([this, Delegate](YvrMessageHandle Message, bool bIsError)
        {
            FYvrPurchase NewPurchase{};
            if (bIsError)
            {
                auto ErrorMessage = OnlineSubsystemYvrWrapper::GetErrorMessage(Message);
                UE_LOG(YvrIAP, Log, TEXT("LaunchCheckoutFlow failed: %s"), *ErrorMessage);
                Delegate.ExecuteIfBound(false);
            }
            else
            {
                auto Purchase = OnlineSubsystemYvrWrapper::GetPurchaseByMessage(Message);
                NewPurchase.Sku = OnlineSubsystemYvrWrapper::GetSkuOfPurchase(Purchase);
                NewPurchase.Name = OnlineSubsystemYvrWrapper::GetNameOfPurchase(Purchase);
                NewPurchase.Id = OnlineSubsystemYvrWrapper::GetTradeNoOfPurchase(Purchase);
                NewPurchase.Type = (EProductType)OnlineSubsystemYvrWrapper::GetTypeOfPurchase(Purchase);
                NewPurchase.Icon = OnlineSubsystemYvrWrapper::GetScoverOfPurchase(Purchase);
                Delegate.ExecuteIfBound(true);
            }

            CurrentPurchase = NewPurchase;
        }));
#endif
}

void FYvrPlatformIAPInterface::GetCachedProducts(TArray<FYvrProduct>& OutProducts)
{
    OutProducts = CachedProducts;
}

void FYvrPlatformIAPInterface::GetCachedPurchases(TArray<FYvrPurchase>& OutPurchases)
{
    OutPurchases = CachedPurchases;
}

void FYvrPlatformIAPInterface::GetCurrentPurchase(FYvrPurchase& OutPurchase)
{
    OutPurchase = CurrentPurchase;
}

FYvrPlatformIAPInterface::FYvrPlatformIAPInterface(FOnlineSubsystemYvr& InSubsystem) :
    YvrSubsystem(InSubsystem)
{

}

FYvrPlatformIAPInterface::~FYvrPlatformIAPInterface()
{

}

