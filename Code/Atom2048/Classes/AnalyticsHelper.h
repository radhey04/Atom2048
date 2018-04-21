// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : June 2017.

#ifndef AnalyticsHelper_hpp
#define AnalyticsHelper_hpp

#include <string>
#include <map>
#include <functional>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
#include "PluginGoogleAnalytics/PluginGoogleAnalytics.h"
#else
#endif

using namespace std;

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
#endif

class AnalyticsHelper
{
public:
    AnalyticsHelper();
    ~AnalyticsHelper();

    static AnalyticsHelper * GetInstance();

    void Initialize();

    void StopAnalyticsSession();
    void StartAnalyticsSession();

    void SetDispatchPeriod(int seconds);

    void SetTestMode(bool enable);

    void SetUser(const std::string & userID);

    void LogScreen(const std::string & title);

    void LogEvent(const std::string & eventCategory,
        const std::string & eventAction,
        const std::string & eventLabel,
        int value);

    void EnableAdvertisingTracking(bool enable);
    void CreateTracker(const std::string & trackerId);

    string GetTrackingId();

    // Using callbacks from AdListener:
    void SetCallbackForAd(string adName, function<void()> &callback);

    // Triggering callbacks:
    void TriggerCallbackForAd(string adName);

private:
    static AnalyticsHelper * _analyticsHelper;
    const string _trackingId = "UA-101831454-1";

    //map<string, function<void()>> _adListenerCallback;

    //map<string, AdInfo> _adInfo;
};

#endif // AnalyticsHelper_hpp