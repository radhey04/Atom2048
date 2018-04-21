// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#include "AnalyticsHelper.h"
#include "cocos2d.h"

USING_NS_CC;

// TBD: What is transaction info @ http://docs.sdkbox.com/en/plugins/googleanalytics/v3-cpp/#use-ecommerce-api-sample
AnalyticsHelper * AnalyticsHelper::_analyticsHelper = new AnalyticsHelper();
AnalyticsHelper::AnalyticsHelper()
{
    log("AnalyticsHelper: constructor runs.");
}

AnalyticsHelper::~AnalyticsHelper()
{
}

AnalyticsHelper * AnalyticsHelper::GetInstance()
{
    log("AnalyticsHelper: GetInstance.");
    if (!_analyticsHelper)
    {
        _analyticsHelper = new AnalyticsHelper();
    }
    return _analyticsHelper;
}

void AnalyticsHelper::Initialize()
{
    log("AnalyticsHelper: Initialize.");

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    // Initializing PluginGoogleAnalytics plugin for ads:
    sdkbox::PluginGoogleAnalytics::init();
    // Creating Listener:
#else

#endif

    // Starting the session:
    StartAnalyticsSession();
}

void AnalyticsHelper::StopAnalyticsSession()
{
    log("AnalyticsHelper: StopAnalyticsSession.");

    // Stop the session of recording events / analytics data:
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::stopSession();
#else

#endif
}

void AnalyticsHelper::StartAnalyticsSession()
{
    log("AnalyticsHelper: StartAnalyticsSession.");

    // Restart the session of recording events / analytics data after your stopped it:
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::startSession();
#else

#endif
}

// Set Repeat Period of sending data from user.
void AnalyticsHelper::SetDispatchPeriod(int seconds)
{
    log("AnalyticsHelper: SetDispatchPeriod = %d", seconds);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::dispatchPeriodically(seconds);
#else

#endif
}

// Enabling test mode:
void AnalyticsHelper::SetTestMode(bool enable)
{
    log("AnalyticsHelper: SetTestMode = %d", enable);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::setDryRun(enable);
#else

#endif
}

void AnalyticsHelper::SetUser(const std::string & userID)
{
    // TBD:
    log("AnalyticsHelper: SetUser = %s", userID.c_str());

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::setUser(userID);
#else

#endif
}

void AnalyticsHelper::LogScreen(const std::string & title)
{
    log("AnalyticsHelper: LogScreen = %s", title.c_str());

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::logScreen(title);
#else

#endif
}

void AnalyticsHelper::LogEvent(const std::string & eventCategory, const std::string & eventAction, const std::string & eventLabel, int value)
{
    // TDB : improve the log:
    log("AnalyticsHelper: LogEvent = %s, %s, %s, %d", eventCategory.c_str(), eventAction.c_str(), eventLabel.c_str(), value);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::logEvent(eventCategory, eventAction, eventLabel, value);
#else

#endif
}

void AnalyticsHelper::EnableAdvertisingTracking(bool enable)
{
    log("AnalyticsHelper: EnableAdvertisingTracking = %d", enable);

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::enableAdvertisingTracking(enable);
#else

#endif
}

void AnalyticsHelper::CreateTracker(const std::string & trackerId)
{
    log("AnalyticsHelper: CreateTracker = %s", trackerId.c_str());

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::PluginGoogleAnalytics::createTracker(trackerId);
#else

#endif
}

string AnalyticsHelper::GetTrackingId()
{
    return _trackingId;
}

// Listener class :
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))

#endif