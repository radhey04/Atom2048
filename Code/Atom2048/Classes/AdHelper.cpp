// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#include "AdHelper.h"
#include "cocos2d.h"
#include "Player.h"
#include "IAPHelper.h"
#include "AnalyticsHelper.h"
#include "AppEvents.h"
#include "Common.h"

USING_NS_CC;

AdHelper * AdHelper::_adHelper = new AdHelper();
AdHelper::AdHelper()
{
}

AdHelper::~AdHelper()
{
}

AdHelper * AdHelper::GetInstance()
{
    if (!_adHelper)
    {
        _adHelper = new AdHelper();
    }
    return _adHelper;
}

void AdHelper::InitializeAds()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    // Initializing AdMob plugin for ads:
    sdkbox::PluginAdMob::init();

    // Cache the Ad items:
    sdkbox::PluginAdMob::cache(_adName.BottomBanner);
    sdkbox::PluginAdMob::cache(_adName.InterstitialTextImage);
    sdkbox::PluginAdMob::cache(_adName.InterstitialVideoOnly);

    // Initializing the AdInfo:
    _adInfo[_adName.BottomBanner] = AdInfo();
    _adInfo[_adName.InterstitialTextImage] = AdInfo();
    _adInfo[_adName.InterstitialVideoOnly] = AdInfo();

    _adInfo[_adName.BottomBanner].AdName = _adName.BottomBanner;
    _adInfo[_adName.InterstitialTextImage].AdName = _adName.InterstitialTextImage;
    _adInfo[_adName.InterstitialVideoOnly].AdName = _adName.InterstitialVideoOnly;

    //// Creating AdListener:
    AdListener * adListener = new AdListener;
    sdkbox::PluginAdMob::setListener(adListener);
#else

#endif
}

void AdHelper::ShowAd(string adName)
{
    log("AdHelper::ShowAd - call entered to show ad : %s", adName.c_str());
    // Checking if the user Already paid for ads removal:
    if (Player::GetInstance()->IsPaidFor(IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds)) {
        log("AdHelper::ShowAd - User has paid to remove ads, so avoid showing ads");
        return;
    }

	// Check if the requested ad is loaded (received response from google servers with ad content).
    if (!((_adInfo.find(adName) != _adInfo.end()) && _adInfo[adName].IsReceived))
    {
        return;
    }

    log("AdHelper::ShowAd - Ad will be shown : %s", adName.c_str());

    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().Ads, AppEvents::GetInstance()->GetEventAction().AdShown, CommonFunc::FirstLetterToUpperCase(adName), 1);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginAdMob::show(adName);
#else
#endif

    // updating the ad count for interstitial ads.
    if ((adName.compare(_adName.InterstitialTextImage) == 0) || (adName.compare(_adName.InterstitialVideoOnly) == 0)) {
        AdHelper::GetInstance()->UpdateInterstitialAdCount(1);
    }
}

void AdHelper::HideAd(string adName)
{
    if (!((_adInfo.find(adName) != _adInfo.end()) && _adInfo[adName].IsReceived))
    {
        return;
    }
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginAdMob::hide(adName);
#else
#endif
}

void AdHelper::RemoveListenerAds()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginAdMob::removeListener();
#else
#endif
}

void AdHelper::ShowInterstitialWeightedAd()
{
    // 1 out of three ads will be shown as a video ad.
    int randomNum = random(1, 8);
    // Google analytics: Date 29-Oct-2017:
    //  After analysing google analytics/AdSense data, the interstitial video ad generates most revenue.
    // Hence all the interstitial ads will now be video ads only.
    if (randomNum <= 4) { // Show a video ad:
        AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().InterstitialVideoOnly);
    }
    else { // Show a text / Image ad.
        AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().InterstitialTextImage);
    }
}

void AdHelper::ShowLateStartAndLightAds()
{
    // Don't show for the first 100 seconds of the game.
    // Then show ad for 1 in every 5 instances. -> 20% times.

    // Edit : Date 30-10-2017 by Radhey Shyam Meena
    // Reason : Analysis of Google ads (admob) data.
    // All the full screen ads will be video ads now, since they are generating more
    // revenue as compared to text/image ads. 
    // The ad frequency will be reduced to not bombard too many ads to the user.

    // Edit : 06-11-2017, Radhey.
    // Update : 60 % of the time when a tile disables, an ad will be shown.
    // Reference : Google Analytics. Ad response and revenue became very low after last update of only video ads, hence a mix of ads will be shown.
    int randomNum = random(1, 10);
    if ((CommonFunc::GetSecondsSince2000() - CommonFunc::GetSessionNumber()) >= 100) {
        if (randomNum <= 5) {
            AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().InterstitialTextImage);
        }
        else {
            AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().InterstitialVideoOnly);
        }
    }
}

AdHelper::AdName AdHelper::GetAdNames()
{
    return _adName;
}

int AdHelper::GetInterstitialAdCount()
{
    return _interstitialAdCount;
}

void AdHelper::SetInterstitialAdCount(int adCount)
{
    _interstitialAdCount = adCount;
}

void AdHelper::UpdateInterstitialAdCount(int adCountDelta)
{
    _interstitialAdCount += adCountDelta;
}

void AdHelper::SetCallbackForAd(string adName, function<void()> &callback)
{
    _adListenerCallback[adName] = callback;
}

void AdHelper::TriggerCallbackForAd(string adName)
{
    log("AdHelper::TriggerCallbackForAd - for ad ( %s ) called from AdListener::adViewDidReceiveAd.", adName.c_str());

    // Setting that the ad has been received and can be shown: First set the IsReceived value and then trigger the callback. 
    _adInfo[adName].IsReceived = true;
    log("AdHelper::TriggerCallbackForAd - (adName, IsReceived) = (%s, %d)", _adInfo[adName].AdName.c_str(), _adInfo[adName].IsReceived);

    if (_adListenerCallback.find(adName) != _adListenerCallback.end()) {
        _adListenerCallback[adName]();
    }
}

// AdListener class :
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))

void AdListener::adViewDidReceiveAd(const std::string &name) {
    AdHelper::GetInstance()->TriggerCallbackForAd(name);
}

void AdListener::adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) {
}

void AdListener::adViewWillPresentScreen(const std::string &name) {
}

void AdListener::adViewDidDismissScreen(const std::string &name) {
}

void AdListener::adViewWillDismissScreen(const std::string &name) {
}

void AdListener::adViewWillLeaveApplication(const std::string &name) {
}

void AdListener::reward(const std::string &name, const std::string &currency, double amount) {
}

#endif