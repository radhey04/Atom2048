// Created by Radhey Shyam Meena
// March 2017.

//
//  Game2048Common.cpp
//
//
#include "Game2048Common.h"
#include "Common.h"
#include "regex"
#include <ctype.h>
#include <chrono>

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
using namespace Windows::System::Profile;
using namespace Windows::Security::Cryptography;
using namespace Platform;
#endif

using namespace rapidjson;
using namespace std;

Game2048Common::NextCallback Game2048Common::_nextCallback = nullptr;
Size Game2048Common::_visibleSize = Director::getInstance()->getVisibleSize();
int Game2048Common::_currentStage = Declarations::Stage::Unknown;
Game2048Common::PlayerDetails Game2048Common::_playerDetails = {};
Game2048Common::Vec2I Game2048Common::ZEROI = Game2048Common::Vec2I(0, 0);
GameTheme * Game2048Common::_gameTheme = new GameTheme();

Game2048Common::Game2048Common() {
}

Game2048Common::~Game2048Common() {
}

void Game2048Common::CleanupLoadedData() {
    Game2048Common::_playerDetails = {};
}

string Game2048Common::currentDateToString()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    string dateString = "", tmp = "";
    tmp = CommonFunc::IntToString(1900 + ltm->tm_year);
    dateString += tmp;
    dateString += "-";
    tmp = CommonFunc::IntToString(1 + ltm->tm_mon);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += "-";
    tmp = CommonFunc::IntToString(ltm->tm_mday);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += " ";
    tmp = CommonFunc::IntToString(ltm->tm_hour);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += ":";
    tmp = CommonFunc::IntToString(1 + ltm->tm_min);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    dateString += ":";
    tmp = CommonFunc::IntToString(1 + ltm->tm_sec);
    if (tmp.length() == 1)
        tmp.insert(0, "0");
    dateString += tmp;
    
    return dateString;
}

bool Game2048Common::IsValidEmailAddress(string email)
{
    // code source: http://etutorials.org/Programming/secure+programming/Chapter+3.+Input+Validation/3.9+Validating+Email+Addresses/
    //  This function tested with following results:
    //    radhey@logicroots.com - shows valid.
    //    .me@example.com - shows valid,
    //    "radhey meena"@logicroots.com - shows invalid.
    //    name\@tag@example.com - shows invalid.
    //
    //    Following emails tested invalid:
    //    me@
    //    @example.com
    //    me.@example.com
    //    me@example..com
    //    me.example@com
    //    me\@example.com
    const char *address = email.c_str();
    
    int        count = 0;
    const char *c, *domain;
    static char *rfc822_specials = "()<>@,;:\\\"[]";
    
    /* first we validate the name portion (name@domain) */
    for(c = address;  *c;  c++) {
        if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) ==
                           '\"')) {
            while (*++c) {
                if (*c == '\"') break;
                if (*c == '\\' && (*++c == ' ')) continue;
                if (*c <= ' ' || *c >= 127) return 0;
            }
            if (!*c++) return 0;
            if (*c == '@') break;
            if (*c != '.') return 0;
            continue;
        }
        if (*c == '@') break;
        if (*c <= ' ' || *c >= 127) return 0;
        if (strchr(rfc822_specials, *c)) return 0;
    }
    if (c == address || *(c - 1) == '.') return 0;
    
    /* next we validate the domain portion (name@domain) */
    if (!*(domain = ++c)) return 0;
    do {
        if (*c == '.') {
            if (c == domain || *(c - 1) == '.') return 0;
            count++;
        }
        if (*c <= ' ' || *c >= 127) return 0;
        if (strchr(rfc822_specials, *c)) return 0;
    } while (*++c);
    
    return (count >= 1);
}

string Game2048Common::removeWhiteSpace(string & str)
{
    string retString = ""; // The input string should not be removeWhiteSpace'd. It would change the struct data otherwise,
    // eg. GamePowers::_powerInfo[basePowerId][0].PowerName -> "Change Question", the value itself changes to "ChangeQuestion", which should not.
    // Hence copying the input string into another one and sending the newly created string after removing whitespace.
    retString = str;
    retString.erase(remove_if(retString.begin(), retString.end(), ::isspace), retString.end());
    return retString;
}

string Game2048Common::unLLIntToString(unsigned long long int value)
{
    char timeString[60];
    sprintf(timeString, "%llu", value);
    return string(timeString);
}

void Game2048Common::subStringReplaceAll(std::string& str, const std::string& fromSubStr, const std::string& toSubStr) {
    if(fromSubStr.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(fromSubStr, start_pos)) != std::string::npos) {
        str.replace(start_pos, fromSubStr.length(), toSubStr);
        start_pos += toSubStr.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

// Game functions:
int Game2048Common::RopeGeneratorRefillTimeInSeconds(int ropeGeneratorLevel){
    return (2 + (2 * (ropeGeneratorLevel+1))) * 60 * 60;
}


int Game2048Common::GetNumberOfDaysSinceBeginning() {
    time_t timeNow = time(0);
    tm * currentTime = localtime(&timeNow);
    log("NoOfDays: Years: %d, noOfDays in current year: %d", currentTime->tm_year, currentTime->tm_yday);
    int totalDaysTillNow = 365 * currentTime->tm_year + currentTime->tm_yday;
    return totalDaysTillNow;
}

int Game2048Common::GetSecondSpentToday(){
    time_t timeNow = time(0);
    tm * currentTime = localtime(&timeNow);
    int remainingTimeHrs = currentTime->tm_hour * 3600;
    int remainingTimeMins = currentTime->tm_min * 60;
    int remainingTimeSecs = currentTime->tm_sec;
    
    return remainingTimeHrs + remainingTimeMins + remainingTimeSecs;
}

bool Game2048Common::SecondStringIsSubStringOfFirst(string baseString, string subString, bool isCaseSensitive) {
    if(!isCaseSensitive){ // if not caseSencitive:
        // to be tested.
        baseString = CommonFunc::ToLowerCase(baseString.c_str());
        subString = CommonFunc::ToLowerCase(subString.c_str());
    }
    size_t found = baseString.find(subString);
    if (found != std::string::npos){
        return true;
    }
    return false;
}

// This function gives local device's clock time in miliSeconds:
long long int Game2048Common::GetMiliSecondsSinceEpoch(){
    return chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch()).count();
}

// Transition from Sunrise to Sunset or vice-versa; Morning to Afternoon etc.
void Game2048Common::StartDayNightTransition(Node * nightBGNode, Declarations::DayTimeTransition dayTimeTransit, float deltaT){ // deltaT in Seconds.
    // Stop any previous runnning transitions.
    nightBGNode->stopAllActions();
    
    int opacityDelta = Declarations::DayTimeTransition::UnknownDayTime;
    switch (dayTimeTransit) {
        case Declarations::DayTimeTransition::Sunset:
            opacityDelta = 1 ; // Transition should be from opacity : 0 -> 255;
            break;
        case Declarations::DayTimeTransition::Sunrise:
            opacityDelta = - 1; // Transition from Opacity: 255 -> 0;
            break;
        default:
            break;
    }
    
    float repeateDeltaT = deltaT/255.0;
    nightBGNode->runAction(RepeatForever::create(Sequence::create(DelayTime::create(repeateDeltaT), CallFunc::create([dayTimeTransit, nightBGNode, opacityDelta](){
        int updatedOpacity = (int)nightBGNode->getOpacity() + opacityDelta; // Definition Opacity: 255 = Full visible; 0 = Full Transparent;
        if (updatedOpacity <=0 && (dayTimeTransit == Declarations::DayTimeTransition::Sunrise)) {
            nightBGNode->stopAllActions();
            return;
        } else if (updatedOpacity >=255 && (dayTimeTransit == Declarations::DayTimeTransition::Sunset)) {
            nightBGNode->stopAllActions();
            return;
        } else if (dayTimeTransit == Declarations::DayTimeTransition::UnknownDayTime) {
            nightBGNode->stopAllActions();
            return;
        }
        nightBGNode->setOpacity(updatedOpacity);
    }), NULL)));
    
}

// Please be a bit more careful while using progressBarCompletion function:
void Game2048Common::ProgressBarCompletion(Node * parentNode, string barName, float percentCompletion){
    percentCompletion = MIN(100.0, MAX(0.0, percentCompletion)); // Limiting the percent values between (0,100);
    float animDuration = 0.500;
    Sprite * leftBar = parentNode->getChildByName<Sprite*>(barName + "Left");
    Sprite * centerBar = parentNode->getChildByName<Sprite*>(barName + "Center");
    Sprite * rightBar = parentNode->getChildByName<Sprite*>(barName + "Right");
    
    // Stop the current actions on all the Bars.
    leftBar->stopAllActions();
    centerBar->stopAllActions();
    rightBar->stopAllActions();
    
    int initializeTag = 1593;
    if(centerBar->getTag() != initializeTag){
        // re-aligning taking the AP into consideration:
        Vec2 resetAnchorPoint = Vec2::ANCHOR_BOTTOM_LEFT;
        leftBar->setPosition(Vec2(leftBar->getPosition().x - leftBar->getAnchorPoint().x * leftBar->getContentSize().width, leftBar->getPosition().y - leftBar->getAnchorPoint().y * leftBar->getContentSize().height));
        leftBar->setAnchorPoint(resetAnchorPoint);
        
        centerBar->setPosition(Vec2(centerBar->getPosition().x - centerBar->getAnchorPoint().x * centerBar->getContentSize().width, centerBar->getPosition().y - centerBar->getAnchorPoint().y * centerBar->getContentSize().height));
        centerBar->setAnchorPoint(resetAnchorPoint);
        centerBar->setScaleX(0.0f);
        
        rightBar->setPosition(Vec2(centerBar->getBoundingBox().origin.x, rightBar->getPosition().y - rightBar->getAnchorPoint().y * rightBar->getContentSize().height) + Vec2(centerBar->getScaleX() * centerBar->getContentSize().width - 2, 0));
        rightBar->setAnchorPoint(resetAnchorPoint);
        rightBar->setFlippedX(true);
        // Tag to Center bar for identification that the positions and AnchorPoints have been correctly reset.
        centerBar->setTag(initializeTag);
    }
    
    if (percentCompletion <= 0.0){
        centerBar->runAction(Spawn::create(TargetedAction::create(leftBar, FadeOut::create(animDuration)),TargetedAction::create(centerBar, FadeOut::create(animDuration)), TargetedAction::create(rightBar, FadeOut::create(animDuration)), NULL));
    } else {
        leftBar->runAction(FadeIn::create(0.0));
        centerBar->runAction(FadeIn::create(0.0));
        rightBar->runAction(FadeIn::create(0.0));
    }
    // The left bar will not require any repositioning or scaling.
    // Center bar re-scaling:
    float centerBarScaleX = (percentCompletion/100.0); // deltaX = the maximum x-posi to which the AP(1,0) of center bar extends/scales.
    centerBar->ignoreAnchorPointForPosition(true);
    centerBar->setAnchorPoint(Vec2(0,0.5));
    FiniteTimeAction * centerBarRescalingAnim = TargetedAction::create(centerBar, ScaleTo::create(animDuration, centerBarScaleX, 1.0)) ;
    
    // Right bar repositioning:
    FiniteTimeAction * rightBarRepositionAnim = TargetedAction::create(rightBar, MoveTo::create(animDuration, Vec2(centerBar->getBoundingBox().origin.x, rightBar->getPosition().y - rightBar->getAnchorPoint().y * rightBar->getContentSize().height) + Vec2(centerBarScaleX * centerBar->getContentSize().width - 2, 0))); // getBoundingBox().origin gives position of AP(0,0).
    centerBar->runAction(Spawn::create(centerBarRescalingAnim, rightBarRepositionAnim, NULL));
    
    log("Game2048Common::ProgressBarCompletion - %s progress bar updated to percent value = %.2f", barName.c_str(), percentCompletion);
}

float Game2048Common::ProgressBarFillerDeltaMove(Node * progressBarFiller, float moveByFraction){
    float deltaXMoveBy = MAX(0.0 - progressBarFiller->getPosition().x, MIN(moveByFraction * progressBarFiller->getContentSize().width,  progressBarFiller->getContentSize().width - progressBarFiller->getPosition().x));
    return deltaXMoveBy;
    //progressBarFiller->runAction(MoveBy::create(Declarations::_progressBarFillTime, Vec2(deltaXMoveBy,0)));
}

ClippingNode * Game2048Common::CreateRectClippingNode(Vec2 anchorPoint, Vec2 position, Size size, Vec2 stencilPosi, Size stencilSize){
    DrawNode * stencil = DrawNode::create();
    Vec2 rectDestination = stencilPosi + Vec2(stencilSize);
    stencil->drawSolidRect(stencilPosi, rectDestination, Color4F(0,0,0,1));
    
    //Clipping node:
    ClippingNode * clipper = ClippingNode::create();
    clipper->setAnchorPoint(anchorPoint);
    clipper->setPosition(position);
    clipper->setInverted(false);
    clipper->setStencil(stencil);
    clipper->setContentSize(size);
    return clipper;
}

ClippingNode * Game2048Common::CreatePolygonClippingNode(cocos2d::Vec2 anchorPoint, cocos2d::Vec2 position, cocos2d::Size size, Vec2 polygonPoints[], unsigned int numOfVertices){
    // Defining Stencil:
    DrawNode * stencil = DrawNode::create();
    stencil->drawSolidPoly(polygonPoints, numOfVertices, Color4F(0,0,0,1));
    
    //Clipping node:
    ClippingNode * clipper = ClippingNode::create();
    clipper->setAnchorPoint(anchorPoint);
    clipper->setPosition(position);
    clipper->setInverted(false);
    clipper->setStencil(stencil);
    return clipper;
}

void Game2048Common::ChangeSpriteImageFromAssets(Sprite * sprite, string fileName){
    /* Create Image */
    Image * newImage = new Image();
    newImage->initWithImageFile( "Assets/" + fileName); /*the fileName is an image file in your Assets folder */
    
    /* Create Texture from Image */
    Texture2D * texture = new Texture2D();
    texture->initWithImage(newImage);
    
    /* Set the Texture */
    sprite->setTexture(texture);
}

string Game2048Common::SecondsToTimeStamp(int totalSeconds, int timeStampPlaces){
    char timeStamp[15];
    int remainingHours = (int)std::floor(totalSeconds /(60 * 60)); // Hours
    int remainingMinutes = (int)std::floor(totalSeconds / (60)) % 60; // minutes
    int remainingSecs = (int)std::floor(totalSeconds % 60); // Seconds.
    sprintf(timeStamp, "%02d:%02d:%02d", remainingHours, remainingMinutes, remainingSecs);
    if (timeStampPlaces == 2) {
        sprintf(timeStamp, "%02d:%02d", remainingMinutes, remainingSecs);
    } else if (timeStampPlaces == 1){
        sprintf(timeStamp, "%02d", remainingHours);
    }
    log("TimeStamp: %s", string(timeStamp).c_str());
    return string(timeStamp);
}