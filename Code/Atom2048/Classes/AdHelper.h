// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#ifndef AdHelper_hpp
#define AdHelper_hpp

#include <string>
#include <map>
#include <functional>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
#include "PluginAdMob/PluginAdMob.h"
#else
#endif

using namespace std;

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
class AdListener : public sdkbox::AdMobListener
{
public:
    virtual void adViewDidReceiveAd(const std::string &name);
    virtual void adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg);
    virtual void adViewWillPresentScreen(const std::string &name);
    virtual void adViewDidDismissScreen(const std::string &name);
    virtual void adViewWillDismissScreen(const std::string &name);
    virtual void adViewWillLeaveApplication(const std::string &name);
    virtual void reward(const std::string &name, const std::string &currency, double amount);
};
#endif

class AdHelper
{
public:
    AdHelper();
    ~AdHelper();

    struct AdName {
        string BottomBanner = "bottomBanner";
        string InterstitialTextImage = "fullScreenImageText";
        string InterstitialVideoOnly = "fullScreenVideoNonRemovable";
    };

    struct AdInfo {
        string AdName = "";
        bool IsReceived = false;
        function<void()> receivedCallback = [] {};
    };

    static AdHelper * GetInstance();

    void InitializeAds();

    void ShowAd(string adName);
    void HideAd(string adName);

    // Remove listener:
    void RemoveListenerAds();

    // this will show 1 video ad in every 3 interstitial ads.
    void ShowInterstitialWeightedAd();

    // Frequency is very less and it would not be shown during the first 100 seconds of the game.
    // Eg. Ad showing while tile disables.
    void ShowLateStartAndLightAds();

    // Ad Name Struct Functions:
    AdName GetAdNames();

    // Ad Count getter / setter:
    int GetInterstitialAdCount();
    void SetInterstitialAdCount(int adCount);
    void UpdateInterstitialAdCount(int adCountDelta);

    // Using callbacks from AdListener:
    void SetCallbackForAd(string adName, function<void()> &callback);

    // Triggering callbacks:
    void TriggerCallbackForAd(string adName);

private:
    static AdHelper * _adHelper;
    
    AdName _adName;
    
    int _interstitialAdCount = 0;

    map<string, function<void()>> _adListenerCallback;

    map<string, AdInfo> _adInfo;
};

#endif // AdHelper_hpp