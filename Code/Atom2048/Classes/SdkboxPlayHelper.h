// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#ifndef SdkboxPlayHelper_hpp
#define SdkboxPlayHelper_hpp

#include <string>
#include <map>
#include <functional>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
#include "PluginSdkboxPlay/PluginSdkboxPlay.h"
#else
#endif

using namespace std;

class SdkboxPlayHelper
{
public:
    SdkboxPlayHelper();
    ~SdkboxPlayHelper();

    // SdkboxPlay Item operation outcome:
    enum SdkboxPlayOutcome
    {
    };

    struct LeaderboardName {
        string HighestScore = "highestscore";
    };

    static SdkboxPlayHelper * GetInstance();

    void InitializeSdkboxPlay();

    void SignIn();

    void SubmitScore(std::string leaderboard_name, int score);

    // Ad Name Struct Functions:
    LeaderboardName GetLeaderboardNames();

    // Display leaderboard:
    void ShowLeaderboard(string leaderboard_name);

    // Using callbacks from AdListener:
    void SetCallbackForSdkboxPlayItem(string SdkboxPlayItemName, SdkboxPlayOutcome iAPOutcome, function<void()> &callback);

    // Make a score transaction:


    // Triggering callbacks:
    void TriggerCallbackForSdkboxPlayItem(string SdkboxPlayItemName, SdkboxPlayOutcome iAPOutcome);

private:
    static SdkboxPlayHelper * _sdkboxPlayHelper;

    LeaderboardName _leaderboardName;
    
    map<string, map<SdkboxPlayOutcome, function<void()>>> _SdkboxPlayIListenerCallback;
};

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
// TBD: Enable listener as applicable;
     //class SdkboxPlayListenerLocal : public sdkbox::SdkboxPlayListener
//{
//public:
//    SdkboxPlayListenerLocal();
//    ~SdkboxPlayListenerLocal();
//
//    virtual void onInitialized(bool ok) override;
//    virtual void onSuccess(sdkbox::Product const& p) override;
//    virtual void onFailure(sdkbox::Product const& p, const std::string &msg) override;
//    virtual void onCanceled(sdkbox::Product const& p) override;
//    virtual void onRestored(sdkbox::Product const& p) override;
//    virtual void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;
//    virtual void onProductRequestFailure(const std::string &msg) override;
//    void onRestoreComplete(bool ok, const std::string &msg);
//}; 

#else
#endif

#endif // SdkboxPlayHelper_hpp