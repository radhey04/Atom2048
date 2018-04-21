// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#include "Common.h"
#include "SdkboxPlayHelper.h"
#include "cocos2d.h"
#include "AnalyticsHelper.h"
#include "AppEvents.h"

USING_NS_CC;

SdkboxPlayHelper * SdkboxPlayHelper::_sdkboxPlayHelper = new SdkboxPlayHelper();
SdkboxPlayHelper::SdkboxPlayHelper()
{
}

SdkboxPlayHelper::~SdkboxPlayHelper()
{
}

SdkboxPlayHelper * SdkboxPlayHelper::GetInstance()
{
    if (!_sdkboxPlayHelper)
    {
        _sdkboxPlayHelper = new SdkboxPlayHelper();
    }
    return _sdkboxPlayHelper;
}

void SdkboxPlayHelper::InitializeSdkboxPlay()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginSdkboxPlay::init();

    // Attaching a listener:
#else

#endif
}

void SdkboxPlayHelper::SignIn()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginSdkboxPlay::signin();
#else

#endif
}

void SdkboxPlayHelper::SubmitScore(std::string leaderboard_name, int score)
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginSdkboxPlay::submitScore(leaderboard_name, score);
#else

#endif
}

SdkboxPlayHelper::LeaderboardName SdkboxPlayHelper::GetLeaderboardNames()
{
    return _leaderboardName;
}

void SdkboxPlayHelper::ShowLeaderboard(string leaderboard_name) {
    bool isSignedIn = false;
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    isSignedIn = sdkbox::PluginSdkboxPlay::isSignedIn();
    if (isSignedIn) {
        sdkbox::PluginSdkboxPlay::showLeaderboard(leaderboard_name);
    }
    else {
        sdkbox::PluginSdkboxPlay::signin();
    }
#else

#endif
    if (!isSignedIn) {
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GPGLogin, AppEvents::GetInstance()->GetEventAction().Initiated, "GPG Login Initiated at Leaderboard", 1);
    }
}


#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
//
//IAPListenerLocal::IAPListenerLocal()
//{
//}
//
//IAPListenerLocal::~IAPListenerLocal()
//{
//}
//
//void IAPListenerLocal::onInitialized(bool ok) {
//    cocos2d::log("IAP: IAPListenerLocal::onInitialized. Status = %d", ok);
//}
//void IAPListenerLocal::onSuccess(sdkbox::Product const& p) {
//    string remarks = "'In app purchase from " + CommonFunc::GetDeviceName() + "'";
//    log("IAP:: Success. %s", remarks.c_str());
//    SdkboxPlayHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, SdkboxPlayHelper::IAPOutcome::Success);
//}
//
//void IAPListenerLocal::onFailure(sdkbox::Product const& p, const std::string &msg) {
//    log("IAP:: Failed - %s", msg.c_str());
//    string remarks = "'In app purchase FAIL from " + CommonFunc::GetDeviceName() + ". " + msg + "'";
//    SdkboxPlayHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, SdkboxPlayHelper::IAPOutcome::Failure);
//}
//
//void IAPListenerLocal::onCanceled(sdkbox::Product const& p) {
//    string remarks = "'In app purchase Canceled from " + CommonFunc::GetDeviceName() + ".'";
//    log("IAP:: Canceled. %s", remarks.c_str());
//    SdkboxPlayHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, SdkboxPlayHelper::IAPOutcome::Canceled);
//}
//
//void IAPListenerLocal::onRestored(sdkbox::Product const& p) {
//    log("IAP:: Restored");
//    SdkboxPlayHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, SdkboxPlayHelper::IAPOutcome::Restored);
//}
//
//void IAPListenerLocal::onProductRequestSuccess(std::vector<sdkbox::Product> const &products) {
//    log("IAP:: Product Request Success :");
//}
//
//void IAPListenerLocal::onProductRequestFailure(const std::string &msg) {
//    log("IAP:: Product Request Failed - %s", msg.c_str());
//}
//
//void IAPListenerLocal::onRestoreComplete(bool ok, const std::string &msg) {
//    log("IAP:: onRestoreComplete. %s", msg.c_str());
//}

#else

#endif
