// Created by Radhey Shyam Meena
// March 2017.

//
//  Game2048Common.hpp
//
//

#ifndef Game2048Common_hpp
#define Game2048Common_hpp

#include "json/document.h"
#include "json/rapidjson.h"
#include "ui/cocosGUI.h"
#include "Declarations.h"
#include "GameTheme.h"

using namespace std;
USING_NS_CC;

class Game2048Common
{
public:
    
    Game2048Common();
    virtual ~Game2048Common();
        
    typedef function<void()> NextCallback;
    
    struct Vec2I {
        int X = 0;
        int Y = 0;
        Vec2I(int x = 0, int y = 0) : X(x), Y(y) {
        }
    };
    static Vec2I ZEROI;

    struct PlayerDetails {
        string Id;
        string Name;
        int CurrentScore = 0;
        int BestScore = 0;
    };

    static PlayerDetails _playerDetails;

    static NextCallback _nextCallback;
    static Size _visibleSize;
    static int _currentStage;

    static GameTheme * _gameTheme;

    static void CleanupLoadedData();

    static string currentDateToString();
    static bool IsValidEmailAddress(string email);
    static string removeWhiteSpace(string & text);
    static string unLLIntToString(unsigned long long int value);
    
    //	static ui::EditBox * createEditBox(string placeholder, string assetName, cocos2d::Size editBoxSize);
    static void subStringReplaceAll(std::string& str, const std::string& fromSubStr, const std::string& toSubStr);
    static int GetNumberOfDaysSinceBeginning();
    static long long int GetMiliSecondsSinceEpoch();
    
    static int GetSecondSpentToday();
    // the id will be unique device id, with which the email is stored as: deviceID@logicroots.com.
    // Below substring check function is not case sensitive:
    static bool SecondStringIsSubStringOfFirst(string baseString, string subString, bool isCaseSensitive);
    
    static void StartDayNightTransition(Node * nightBGNode, Declarations::DayTimeTransition dayTimeTransit, float deltaT);
    
    static void ProgressBarCompletion(Node * parentNode, string barName, float percentCompletion);
    static float ProgressBarFillerDeltaMove(Node * progressBarFiller, float moveByFraction);

    static ClippingNode * CreateRectClippingNode(Vec2 anchorPoint, Vec2 position, Size size, Vec2 stencilPosi, Size stencilSize);
    static ClippingNode * CreatePolygonClippingNode(Vec2 anchorPoint, Vec2 position, Size size, Vec2 polygonPoints[], unsigned int numOfVertices);
    static void ChangeSpriteImageFromAssets(Sprite * sprite, string fileName);
    
    static string SecondsToTimeStamp(int totalSeconds, int timeStampPlaces = 3);
    
    // Game Specific Functions:
    static int RopeGeneratorRefillTimeInSeconds(int ropeGeneratorLevel);
    
};

#endif
/* Game2048Common_hpp */
