// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#ifndef IAPHelper_hpp
#define IAPHelper_hpp

#include <string>
#include <map>
#include <functional>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
#include "PluginIAP/PluginIAP.h"
#else
#endif

using namespace std;

class IAPHelper
{
public:
    IAPHelper();
    ~IAPHelper();

    // IAP Item operation outcome:
    enum IAPOutcome
    {
        Initialized,
        Success,
        Failure,
        Canceled,
        Restored,
        ProductRequestSuccess,
        ProductRequestFailure,
        RestoreComplete
    };

    struct IAPItemName {
        string RemoveAds = "RemoveAds";
        string UnlockNormalMode = "UnlockNormalMode";
        string BuyXDiffusers = "Buy5Diffusers";
        string BuyYDiffusers = "Buy40Diffusers";
    };

    struct IAPProduct {
        string name;
        float priceValue;
        string title;
    };

    static IAPHelper * GetInstance();

    void InitializeIAP();

    // Restoring already purchased items from google play:
    void RestoreIAP();

    // Ad Name Struct Functions:
    IAPItemName GetIAPItemNames();

    // Using callbacks from AdListener:
    void SetCallbackForIAPItem(string IAPItemName, IAPOutcome iAPOutcome, function<void()> &callback);

    // Buying and IAP item:
    // For a managed item, duration = 0;
    void BuyIAPItem(string IAPItemName, int duration);

    // Check if an item has already been bought or not:
    bool IsPaid(string IAPItemName);
    
    // Set the paid value of an IAP item:
    void SetPaid(string IAPItemName, bool value);

    // Triggering callbacks:
    void TriggerCallbackForIAPItem(string IAPItemName, IAPOutcome iAPOutcome);

    vector<IAPProduct> GetIAPProducts();

private:
    static IAPHelper * _IAPHelper;

    IAPItemName _IAPItemName;
    
    map<string, map<IAPOutcome, function<void()>>> _IAPIListenerCallback;

    vector<IAPProduct> _IAPProducts;
};

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))

class IAPListenerLocal : public sdkbox::IAPListener
{
public:
    IAPListenerLocal();
    ~IAPListenerLocal();

    virtual void onInitialized(bool ok) override;
    virtual void onSuccess(sdkbox::Product const& p) override;
    virtual void onFailure(sdkbox::Product const& p, const std::string &msg) override;
    virtual void onCanceled(sdkbox::Product const& p) override;
    virtual void onRestored(sdkbox::Product const& p) override;
    virtual void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;
    virtual void onProductRequestFailure(const std::string &msg) override;
    void onRestoreComplete(bool ok, const std::string &msg);
}; 

#else
#endif

#endif // IAPHelper_hpp