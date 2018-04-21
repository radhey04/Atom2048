// Created by Radhey Shyam Meena
// March 2017.

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "PlatformInterface.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include "cocos2d.h"

using namespace std;

//#define  CLASS_NAME "org/cocos2dx/cpp/Cocos2dxActivity"
#define  CLASS_NAME "org/cocos2dx/cpp/AppActivity"

static bool getJNIStaticMethodInfo(cocos2d::JniMethodInfo &methodinfo,
    const char *methodName,
    const char *paramCode) {
    return cocos2d::JniHelper::getStaticMethodInfo(methodinfo,
        CLASS_NAME,
        methodName,
        paramCode);
}

LocalNotification::LocalNotification()
{
}

LocalNotification::~LocalNotification()
{
}

void LocalNotification::show(std::string message, int interval, int tag)
{
    cocos2d::JniMethodInfo methodInfo;
    cocos2d::log("The message in cpp is: %s", message.c_str());
    if (!getJNIStaticMethodInfo(methodInfo, "showLocalNotification", "(Ljava/lang/String;II)V")) {
        return;
    }

    jstring stringArg = methodInfo.env->NewStringUTF(message.c_str());
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg, interval, tag);
    methodInfo.env->DeleteLocalRef(stringArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void LocalNotification::cancel(int tag)
{
    cocos2d::JniMethodInfo methodInfo;

    if (!getJNIStaticMethodInfo(methodInfo, "cancelLocalNotification", "(I)V")) {
        return;
    }

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, tag);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

std::string AncillaryClass::getUUID()
{
    string uuidString = "NA";
    cocos2d::JniMethodInfo methodInfo; // define the function information structure Jni

    CCLOG("AncillaryClass::getUUID funcaion called");

    if (!getJNIStaticMethodInfo(methodInfo, "getUUID", "()Ljava/lang/String;")) {
        cocos2d::log("Invalid Method call to java from cpp function AncillaryClass::getUUID");
        return uuidString;
    }

    jstring jstr = (jstring)(methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID));
    uuidString = cocos2d::JniHelper::jstring2string(jstr);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(jstr);

    CCLOG("AncillaryClass::getUUID : UUID is :%s", uuidString.c_str());
    return uuidString;
}

std::string AncillaryClass::getDeviceName(){
    cocos2d::JniMethodInfo methodInfo;
    cocos2d::log("Getting device name");
    std::string str = "NA";
    if (!getJNIStaticMethodInfo(methodInfo, "getDeviceName", "()Ljava/lang/String;")) {
		cocos2d::log("Invalid Method call to java from cpp function AncillaryClass::getDeviceName");
        return str;
    }
    
    jstring jstr = (jstring)(methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID));
    str = cocos2d::JniHelper::jstring2string(jstr);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(jstr);
    
    cocos2d::log("Device name is: %s", str.c_str());
    return str;
}


std::string AncillaryClass::getVersionName()
{
    cocos2d::JniMethodInfo methodInfo;
    cocos2d::log("Getting versionName");
    std::string versionName = "NA";
    if (!getJNIStaticMethodInfo(methodInfo, "getVersionName", "()Ljava/lang/String;")) {
        cocos2d::log("Invalid Method call to java from cpp function AncillaryClass::getVersionName");
        return versionName;
    }

    jstring jstr = (jstring)(methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID));
    versionName = cocos2d::JniHelper::jstring2string(jstr);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(jstr);

    cocos2d::log("Android versionName is: %s", versionName.c_str());
    return versionName;
}

std::string AncillaryClass::getVersionCode()
{
    cocos2d::JniMethodInfo methodInfo;
    cocos2d::log("Getting versionCode");
    std::string versionCode = "NA";
    if (!getJNIStaticMethodInfo(methodInfo, "getVersionCode", "()Ljava/lang/String;")) {
        cocos2d::log("Invalid Method call to java from cpp function AncillaryClass::getVersionCode");
        return versionCode;
    }

    jstring jstr = (jstring)(methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID));
    versionCode = cocos2d::JniHelper::jstring2string(jstr);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(jstr);

    cocos2d::log("Android versionCode is: %s", versionCode.c_str());
    return versionCode;
}

void AncillaryClass::text2Speech(std::string statement) {
    cocos2d::JniMethodInfo methodInfo;

    if (!getJNIStaticMethodInfo(methodInfo, "text2Speech", "(Ljava/lang/String;)V")) {
        return;
    }
    jstring stringArg = methodInfo.env->NewStringUTF(statement.c_str());
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
    methodInfo.env->DeleteLocalRef(stringArg);
}

void AncillaryClass::consumeIAPProducts() {
    cocos2d::JniMethodInfo methodInfo;

    if (!getJNIStaticMethodInfo(methodInfo, "consumeIAPProducts", "()V")) {
        return;
    }
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void AncillaryClass::hideSystemUI() {
    cocos2d::JniMethodInfo methodInfo;

    if (!getJNIStaticMethodInfo(methodInfo, "hideSoftKeyboard", "()V")) {
        return;
    }
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void AncillaryClass::callShare() {
    cocos2d::JniMethodInfo methodInfo;

    if (!getJNIStaticMethodInfo(methodInfo, "callShare", "()V")) {
        cocos2d::log("Invalid Method call to java from cpp function AncillaryClass::callShare");
        return;
    }

    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

    cocos2d::log("AncillaryClass: callShare called on android.");
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID