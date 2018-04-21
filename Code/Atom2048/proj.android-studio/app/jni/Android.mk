LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH))

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../../Classes/AppDelegate.cpp \
../../../Classes/GameTheme.cpp \
../../../Classes/PlatformInterface.cpp \
../../../Classes/SubSquareContainer.cpp \
../../../Classes/Common.cpp \
../../../Classes/Declarations.cpp \
../../../Classes/Game2048Common.cpp \
../../../Classes/Game2048Scene.cpp \
../../../Classes/GameLevels.cpp \
../../../Classes/XTLayer.cpp \
../../../Classes/SdkboxPlayHelper.cpp \
../../../Classes/AdHelper.cpp \
../../../Classes/IAPHelper.cpp \
../../../Classes/Player.cpp \
../../../Classes/AnalyticsHelper.cpp \
../../../Classes/AppEvents.cpp \
../../../Classes/GameSound.cpp \
../../../Classes/GamerDevice.cpp \
../../../Classes/KeyNames.cpp

LOCAL_CPPFLAGS := -DSDKBOX_ENABLED
LOCAL_LDLIBS := -landroid \
-llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes
LOCAL_WHOLE_STATIC_LIBRARIES := PluginSdkboxPlay \
sdkbox \
PluginIAP \
android_native_app_glue \
PluginAdMob \
PluginGoogleAnalytics

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginsdkboxplay)
$(call import-module, ./pluginiap)
$(call import-module, ./pluginadmob)
$(call import-module, ./plugingoogleanalytics)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
