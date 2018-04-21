// Created by Radhey Shyam Meena
// March 2017.

#include "Common.h"
#include "PlatformInterface.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include "tinyxml2/tinyxml2.h"
#include <vector>
#include <cctype>
#include <iomanip>
#include <algorithm>

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT))
#include <windows.h>
#include <wininet.h>
#endif

USING_NS_CC;
using namespace std;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
Size CommonFunc::_designResolutionSize = Size(768, 1366);
#else
Size CommonFunc::_designResolutionSize = Size(2417, 1366);
#endif

const string CommonFunc::_soundPath = "Assets/Sound/";
const string CommonFunc::_fontPath = "fonts/";
long int CommonFunc::_sessionNumber;

CommonFunc::CommonFunc()
{
}

CommonFunc::~CommonFunc()
{
}

void CommonFunc::loadAssetDetails(string fileName, map<string, map<string, float>> * xmlLoader)
{
    // Reading the xml file from assets folder relative location into string:
    string fileDataString = FileUtils::getInstance()->getStringFromFile(("Assets/" + fileName).c_str());
    //log("Android App XML file string is : %s .", fileDataString.c_str());
    
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    tinyxml2::XMLError errorId = pDoc->Parse(fileDataString.c_str());
    
    if (errorId != 0) {
        return;
    }
    
    tinyxml2::XMLElement *rootEle = pDoc->RootElement();
    
    tinyxml2::XMLElement *layerElement = rootEle->FirstChildElement("layer");
    
    while ( layerElement ) {
        map<string, float> assetAtrribute;
        const tinyxml2::XMLAttribute *attribute = layerElement->FirstAttribute()->Next();
        while (attribute) {
            if(strcmp(attribute->Name(), "position") == 0)
            {
                char str[50];
                strncpy(str, attribute->Value(), sizeof(str));
                char* parts[2] = {0};
                parts[0] = strtok(str,",");
                parts[1] = strtok(0, ",");
                assetAtrribute["x"] = CommonFunc::StringToInt(parts[0]);
                assetAtrribute["y"] = CommonFunc::StringToInt(parts[1]);
            }
            else if(!(strcmp(attribute->Name(), "transformpoint") == 0))
                assetAtrribute[attribute->Name()] = CommonFunc::StringToInt(attribute->Value());
            log("attribute_name = %s,attribute_value = %s", attribute->Name(), attribute->Value());
            attribute = attribute->Next();
        }
        (*xmlLoader)[layerElement->GetText()] = assetAtrribute;
        log("childEle Text= %s", layerElement->GetText());
        layerElement = layerElement->NextSiblingElement();
    }
    
    delete pDoc;
    
}

Layer * CommonFunc::CreateLayerColor(Vec2 anchorPoint, Vec2 position, Size layerSize, Color4B color, string layerName)
{
    Layer * layerColor = LayerColor::create(color);
    layerColor->ignoreAnchorPointForPosition(false);
    layerColor->setAnchorPoint(anchorPoint);
    layerColor->setPosition(position);
    layerColor->setContentSize(layerSize);
    layerColor->setName(layerName);
    return layerColor;
}

void CommonFunc::scheduleNotification(int timeInterval, string mesg, int tag)
{
#ifdef WIN32
    log("Windows: Scheduling notification.");
#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))
    log("MAC");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    log("Android: scheduleNotification");
    //LocalNotification::show(mesg, timeInterval, tag);
#else
    log("CommonFunc: Scheduling notification.");
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
    createAndScheduleLocalNotification(timeInterval, mesg, tag);
#endif
#endif
}

void CommonFunc::cancelNotification(int tag){
#ifdef WIN32
    log("Windows: cancelNotification.");
#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))
    log("MAC");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    log("Android: cancelNotification");
    //LocalNotification::cancel(tag);
#else
    log("CommonFunc: cancelNotification.");
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
    cancelScheduleNotification(tag);
#endif
#endif
}

void CommonFunc::Text2Speech(std::string statement){
#ifdef WIN32
    log("Windows: cancelNotification.");
#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))
    log("MAC");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    log("Android: cancelNotification");
#else
    log("CommonFunc: cancelNotification.");
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#endif
#endif
}

void CommonFunc::StopText2Speech(){
#ifdef WIN32
    log("Windows: cancelNotification.");
#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))
    log("MAC");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    log("Android: cancelNotification");
#else
    log("CommonFunc: cancelNotification.");
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#endif
#endif
}

Label* CommonFunc::createLabel(string displayString, string fontFilePath, float fontSize, float startX, float startY,
    Color3B textColor, TextHAlignment textHAlign, TextVAlignment textVAlign)
{
    log("CommonFunc::createLabel : Creating with string: %s", displayString.c_str());
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto label = Label::createWithTTF(displayString, CommonFunc::_fontPath + fontFilePath, fontSize);

    label->setAnchorPoint(Vec2(0, 0));
    label->setColor(textColor);
    label->setAlignment(textHAlign);
    label->setVerticalAlignment(textVAlign);
    label->setPosition(Vec2(origin.x + startX, origin.y + startY));
    label->setName(displayString);

    return label;
}

Sprite* CommonFunc::createSpriteFromCache(string assetName, float startX, float startY, string assetIDName)
{
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(assetName);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::createWithSpriteFrame(spriteFrame);

    // position the sprite on the center of the screen
    sprite->setAnchorPoint(Vec2(0, 0));
    sprite->setPosition(Vec2(origin.x + startX, origin.y + startY));
    sprite->setName(assetIDName);
    log("Sprite created: %s . origin.X: %.2f, origin.y: %.2f, startX: %.2f,  startY: %.2f", assetName.c_str(), origin.x, origin.y, startX, startY);
    sprite->setTag(0);

    return sprite;
}

void CommonFunc::changeSpriteImage(Sprite* sprite, string fileName)
{
    log("CommonFunc::changeSpriteImage : Sprite Name : %s ; Image Name : %s", sprite->getName().c_str(), fileName.c_str());
    sprite->setDisplayFrame(SpriteFrameCache::getInstance()->spriteFrameByName(fileName));
}

void CommonFunc::loadSpriteOnScreen(Layer * layer, vector<vector<string>> * assetToLoad,
                                    map<string, map<string, float>> * loadedXmlData) {
    
    Size visibleSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize(); // 1366x768 Or 1024x768 or 2048x1536
    log("Device Resolution is: %.0fx%.0f", visibleSize.width, visibleSize.height);
    log("Design Resolution is: %.0fx%.0f", _designResolutionSize.width, _designResolutionSize.height);

    // Storing the value of the char arrays in strings:
    string assetSpriteName;
    string assetNameInXml;
    string assetAlign;
    
    vector<vector<string>>::iterator row;
    int i = 0;
    
    for (row = assetToLoad->begin(); row != assetToLoad->end(); row++) {
        
        assetSpriteName = (*assetToLoad)[i][1].c_str();
        assetNameInXml = (*assetToLoad)[i][2].c_str();
        assetAlign = (*assetToLoad)[i][3].c_str();
        // sprite name validation check:
        if (assetSpriteName == "") {
            log("Sprite name is nil");
            i++;
            continue;
        }
        
        log("Sprite Accessed = %s", (*assetToLoad)[i][1].c_str());
        
        // Required parameters: screen location, z-index.
        char str[50];
        sprintf(str, "%s.png", (*assetToLoad)[i][1].c_str());
        
        Vec2 startPosition = CommonFunc::getAlignRelativePosition(assetNameInXml, loadedXmlData, assetAlign);

        auto drawSprite = CommonFunc::createSpriteFromCache(str, startPosition.x, startPosition.y, (*assetToLoad)[i][0].c_str());
        
        if (assetAlign == "RIGHT") {
            // Get the design resolution and device-resolution from the AppDelegate.cpp
            drawSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
        }
        else if (assetAlign == "CENTER") {
            // Get the design resolution and device-resolution from the AppDelegate.cpp
            drawSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        }
        else if (assetAlign == "LEFT") { // assetAlign =="LEFT" by default.
            drawSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        }
        else if (assetAlign == "LEFTCENTER") {
            drawSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        }
        else if (assetAlign == "TOP") {
            drawSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        }

        layer->addChild(drawSprite, (*loadedXmlData)[assetNameInXml]["stack"]);
        
        i++;
    }
}

// To blink a sprite
void CommonFunc::spriteBlink(Sprite *sprite, float timeDelay, int iterations)
{
    auto fadeOutAction = FadeOut::create(timeDelay / 2.0);
    auto fadeInAction = FadeIn::create(timeDelay / 2.0);
    
    if (iterations == -1) {
        sprite->runAction(RepeatForever::create(Sequence::create(fadeOutAction, fadeInAction, NULL)));
    }
    else {
        sprite->runAction(Repeat::create(Sequence::create(fadeOutAction, fadeInAction, NULL), iterations));
    }
}

void CommonFunc::RedirectToAppStore(string appID)
{
    string redirectAddress = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    redirectAddress = "ms-windows-store://review/?ProductId=" + appID;
    // Example of OneNote: ms-windows-store://review/?ProductId=9WZDNCRFHVJL
#elif( (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) )
    redirectAddress = "http://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?id="+
    appID +"&pageNumber=0&sortOrdering=2&type=Purple+Software&mt=8";
    // Example of Plus10:
    //	"http://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?id=1129179941&pageNumber=0&sortOrdering=2&type=Purple+Software&mt=8";
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    string deviceName = AncillaryClass::getDeviceName();
    if (deviceName == "KINDLE") { // redirect to Amazon Store.
        //"http://www.amazon.com/gp/mas/dl/android?p=com.yapp.blah"
        // "amzn://apps/android?p=package_name"
        redirectAddress = "http://www.amazon.com/gp/mas/dl/android?p=" + appID;
    }
    else { // redirect to Google Play store:
        redirectAddress = "https://play.google.com/store/apps/details?id=" + appID;
    }
#else
    redirectAddress = "http://logicroots.com/" + appID;
#endif
    
    Application::getInstance()->openURL(redirectAddress);
}

void CommonFunc::RedirectToAppStoreDetailsTab(string appID){
    string redirectAddress = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    redirectAddress = "ms-windows-store://review/?ProductId=" + appID;
    // Example of OneNote: ms-windows-store://review/?ProductId=9WZDNCRFHVJL
#elif( (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) )
    redirectAddress = "http://itunes.apple.com/app/id" + appID;
    // Example of Mathaly:
    // http://itunes.apple.com/app/id1158780595
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    string deviceName = AncillaryClass::getDeviceName();
    if (deviceName == "KINDLE") { // redirect to Amazon Store.
        //"http://www.amazon.com/gp/mas/dl/android?p=com.yapp.blah"
        // "amzn://apps/android?p=package_name"
        redirectAddress = "http://www.amazon.com/gp/mas/dl/android?p=" + appID;
    }
    else { // redirect to Google Play store:
        redirectAddress = "https://play.google.com/store/apps/details?id=" + appID;
    }
#else
    redirectAddress = "http://logicroots.com/" + appID;
#endif
    
    Application::getInstance()->openURL(redirectAddress);
}

void CommonFunc::CallShare() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))

#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AncillaryClass::callShare();
#else

#endif
}

void CommonFunc::ConsumeIAPProducts() {
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)

#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))

#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Amazon / Play Store differentiation will take place in JavaCode.
    AncillaryClass::consumeIAPProducts();
#else
#endif
}

string CommonFunc::IntToString(int value)
{
    ostringstream os ;
    os << value ;
    return os.str() ;
}

int CommonFunc::StringToInt(string value) {
    return atoi(value.c_str());
}

double CommonFunc::StringToFloat(const char * nptr)
{
    return (strtod(nptr, NULL));
}

vector<string> CommonFunc::SplitString(string str, char delim, int rep){
    vector<string> splittedString = {};
    string buf = "";
    int i = 0;
    while (i < str.length()) {
        if (str[i] != delim)
            buf += str[i];
        else if (rep == 1) {
            splittedString.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            splittedString.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        splittedString.push_back(buf);
    return splittedString;
}

long int CommonFunc::GetSecondsSince2000()
{
    long int seconds = time(0) - (30 * 365 * 24 * 60 * 60 + 7 * 24 * 60 * 60); // years: 2000-1970 = 30
    return seconds;
}


string CommonFunc::Num2Alpha(int n, bool capital)
{
    assert(n >= 1 && n <= 26);
    if(capital){
        string alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        return alphabets.substr(n-1,1);
    }
    else{
        string alphabets = "abcdefghijklmnopqrstuvwxyz";
        return alphabets.substr(n-1,1);
    }
}

string CommonFunc::GetUUID()
{
    // Checking is uuid was saved:
    string userUuid = UserDefault::getInstance()->getStringForKey("UserUUID", "");
    if (!userUuid.empty() && userUuid.length() == 36) {
        return userUuid;
    }

#ifdef WIN32
    GUID guid;

    CoCreateGuid(&guid);
    OLECHAR* bstrGuid;
    StringFromCLSID(guid, &bstrGuid);

    // use bstrGuid...
    string s((char*)bstrGuid);

    // ensure memory is freed
    ::CoTaskMemFree(bstrGuid);
    userUuid = s;
#elif((CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX))

    uuid_t uuid;
    uuid_generate_random(uuid);
    char temp[37];
    uuid_unparse(uuid, temp);
    string s(temp);
    userUuid = s;
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::string uuidAndroid = AncillaryClass::getUUID();
    log("UUID for android device: %s", uuidAndroid.c_str());
    userUuid = uuidAndroid;
#else
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
    userUuid = getUniqueIdentifier();
#endif

#endif

    // Save and return the uuid:
    UserDefault::getInstance()->setStringForKey("UserUUID", userUuid);
    log("CommongFunc::GetUUID returns : %s", userUuid.c_str());
    return userUuid;
}

string CommonFunc::unLongIntToString(unsigned long int value)
{
    ostringstream os;
    os << value;
    return os.str();
}

string CommonFunc::GetDeviceName() {
    string deviceName;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    //Code for windows:
    deviceName = "WIN";
#elif( (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) )
    //Code for Mac:
    deviceName = "IPAD";
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //Code for Android:
    deviceName = AncillaryClass::getDeviceName();
#else
    //Code otherwise:
    deviceName = "OTHERS";
#endif
    
    return deviceName;
}

Vec2 CommonFunc::getAlignRelativePosition(string assetNameInXml, map<string, map<string, float>> * loadedXmlData, string assetAlign) {
    Size visibleSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();

    float startX = (*loadedXmlData)[assetNameInXml]["x"],
        startY = (*loadedXmlData)[assetNameInXml]["y"],
        assetWidth = (*loadedXmlData)[assetNameInXml]["width"],
        assetHeight = (*loadedXmlData)[assetNameInXml]["height"]
        ;
    if (assetAlign == "RIGHT") {
        // Get the design resolution and device-resolution from the AppDelegate.cpp
        startX = visibleSize.width - _designResolutionSize.width + (startX + assetWidth);
    }
    else if (assetAlign == "CENTER") {
        // Get the design resolution and device-resolution from the AppDelegate.cpp
        startX = visibleSize.width / 2 - _designResolutionSize.width / 2 + (startX + assetWidth / 2);
    }
    else if (assetAlign == "LEFT") { // assetAlign =="LEFT" by default.
        startX = startX;
    }
    else if (assetAlign == "LEFTCENTER") {
        startY = visibleSize.height / 2.0 - _designResolutionSize.height / 2.0 + (startY + assetHeight);
    }
    else if (assetAlign == "TOP") {
        startY = visibleSize.height - _designResolutionSize.height + (startY + assetHeight);
    }
    
    return Vec2(startX, startY);
}

Layer * CommonFunc::createLayer(Vec2 anchorPoint, Vec2 position, Size layerSize)
{
    Layer * layer = Layer::create();
    layer->ignoreAnchorPointForPosition(false); // Since the anchor point is provided in function.
    layer->setAnchorPoint(anchorPoint);
    layer->setPosition(position);
    layer->setContentSize(layerSize);
    return layer;
}

// world position with respect to sprite's anchor point(0,0)
Rect CommonFunc::getWorldSpaceBoundingBox(Node * node)
{
    Vec2 nodeAnchorPoint = node->getAnchorPoint();
    Size nodeSize = node->getContentSize();
    Vec2 nodePosition = node->getPosition();
    //Rect localBoundingBox = node->getBoundingBox();
    Vec2 localPosition = Vec2(nodePosition.x - nodeAnchorPoint.x * nodeSize.width, nodePosition.y - nodeAnchorPoint.y * nodeSize.height); // position on point (0,0) of sprite.
    if (!(node->getParent())) {
        log("CommonFunc::getWorldSpaceBoundingBox(Node * node) returns nullptr: Parent of node is null.");
        return Rect();
    }
    Vec2 worldPosition = node->getParent()->convertToWorldSpace(localPosition);
    Rect nodeRect = Rect(worldPosition.x, worldPosition.y, nodeSize.width, nodeSize.height);
    return nodeRect;
}

string CommonFunc::ToUpperCase(const char * text)
{
    string str(text);
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

string CommonFunc::ToLowerCase(const char * text)
{
    string str(text);
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

string CommonFunc::FirstLetterToUpperCase(string text)
{
    text[0] = toupper(text[0]);
    return text;
}

// class TextInputField implementation:
ui::TextField * CommonFunc::createTextField(string placeHolder, Size textAreaDimension, int fontSize, string fontFace)
{
    ui::TextField * textInputField = ui::TextField::create();
    textInputField->setPlaceHolder(placeHolder);
    textInputField->setFontSize(fontSize);
    //textInputField->setString(placeHolder);
    textInputField->setContentSize(textAreaDimension);
    textInputField->setPosition(Vec2());
    textInputField->setTextColor(Color4B(0, 0, 0, 255));
    textInputField->setAnchorPoint(Vec2(0, 0));
    textInputField->setInsertText(true);
    textInputField->setTextHorizontalAlignment(TextHAlignment::LEFT);
    textInputField->setTextVerticalAlignment(TextVAlignment::BOTTOM);
    textInputField->setFontName(CommonFunc::_fontPath + fontFace);
    return textInputField;
}

void CommonFunc::StartSession()
{
    CommonFunc::_sessionNumber = CommonFunc::GetSecondsSince2000();
}

long int CommonFunc::GetSessionNumber()
{
    return CommonFunc::_sessionNumber;
}
