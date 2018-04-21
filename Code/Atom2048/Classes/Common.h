// Created by Radhey Shyam Meena
// March 2017.

#ifndef __COMMON_FUNC_H__
#define __COMMON_FUNC_H__

// Edited by Radhey Shyam Meena, April 2016 - continued.
// Copyrights reserved with LR-Labs. LogicRoots Pvt. Ltd.

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"
#include "editor-support/spine/SkeletonAnimation.h"
#include <string>
#include <sstream>

using namespace std;
using namespace cocos2d;
extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#elif ( (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) )
#include <uuid/uuid.h>
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#else
    
#endif
}

class CommonFunc
{
public:    
    static Size _designResolutionSize;

    static const string _soundPath, _fontPath;

    CommonFunc();
    virtual ~CommonFunc();
    
    //Methods
    static void loadAssetDetails(string fileName, map<string, map<string, float>> * xmlLoader);

    static Layer * CreateLayerColor(Vec2 anchorPoint, Vec2 position, Size layerSize, Color4B color, string layerName);
    
    static void scheduleNotification(int timeInterval, string mesg, int tag);
    
    static void cancelNotification(int tag);
    
    static void Text2Speech(std::string statement);
    static void StopText2Speech();

    static Label* createLabel(string displayString, string fontFilePath, float fontSize, float startX, float startY,
        Color3B textColor, TextHAlignment textHAlign, TextVAlignment textVAlign);

    static Sprite* createSpriteFromCache(string assetName, float startX, float startY, string assetIDName);
    static void loadSpriteOnScreen(Layer * layer, vector<vector<string>> * assetToLoad,
        map<string, map<string, float>> * loadedXmlData);

    static void changeSpriteImage(Sprite* sprite, string assetName);

    static void spriteBlink(Sprite *sprite, float timeDelay, int iterations);

    static void RedirectToAppStore(string appID);
    static void RedirectToAppStoreDetailsTab(string appID);
    
    static void CallShare();

    static void ConsumeIAPProducts();

    // Various functions:
    static vector<string> SplitString(string myString, char delim, int rep);
    
    static long int GetSecondsSince2000();

    static string IntToString(int value);
    static int StringToInt(string value);
    static double StringToFloat(const char *nptr);
    
    // Unsigned long int to String
    static string unLongIntToString(unsigned long int value);
    
    static string Num2Alpha(int n, bool capital);
    
    static string GetUUID();

    static string GetDeviceName();
    
    static Vec2 getAlignRelativePosition(string assetNameInXml, map<string, map<string, float>> * loadedXmlData, string assetAlign);
    
    static Layer * createLayer(Vec2 anchorPoint, Vec2 position, Size layerSize);
    
    static Rect getWorldSpaceBoundingBox(Node * node);
    
    static string ToUpperCase(const char *text);
    static string ToLowerCase(const char * text);
    static string FirstLetterToUpperCase(string text);
    
    static ui::TextField * createTextField(string placeHolder, Size textAreaDimension, int fontSize, string fontFace);

    // Starting a Session:
    static void StartSession();
    static long int GetSessionNumber();
    
private:
    static long int _sessionNumber;

};


#endif // __COMMON_FUNC_H__
