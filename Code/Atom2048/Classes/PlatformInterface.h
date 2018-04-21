// Created by Radhey Shyam Meena
// March 2017.

#ifndef __PLATFORM_INTERFACE_H__
#define __PLATFORM_INTERFACE_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include <string>

class LocalNotification {
public:
    /**
    * Show local notification
    * @param message  Message should be shown in notificatin
    * @param interval Interval in seconds
    * @param tag      Tag to specify notification
    */
    static void show(std::string message, int interval, int tag);

    /**
    * Cancel local notification specified by ID
    * @param tag Tag of local notification
    */
    static void cancel(int tag);
    
    LocalNotification();
    virtual ~LocalNotification();
};

class AncillaryClass {
public:
    AncillaryClass();
    virtual ~AncillaryClass();
    
    static std::string getUUID();
    static std::string getCountryCode();
    static std::string getDeviceName();
    static std::string getVersionName();
    static std::string getVersionCode();
    static void text2Speech(std::string statement);
    static void consumeIAPProducts();

    static void hideSystemUI();

    static void callShare();
};

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#endif // __PLATFORM_INTERFACE_H__