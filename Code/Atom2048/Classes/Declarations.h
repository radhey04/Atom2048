// Created by Radhey Shyam Meena
// March 2017.

#ifndef  _DECLARATIONS_H_
#define  _DECLARATIONS_H_

// created by Radhey Shyam Meena. 06/Aug/2016.

// Asset declations
#include "cocos2d.h"
#include <cstring>
#include <vector>
#include "json/rapidjson.h"
#include "json/document.h"

using namespace std;
USING_NS_CC;

class Declarations {
public:
    Declarations();
    virtual ~Declarations();
    
    // App release data:
    // Type: String or vector<string>.
    static string _productId, _AppID, _productName;
    static string _errorGenericMessage, _errorConnectionTimedOutMessage, _errorUnspecifiedErrorMessage;
    static string _registrationTitleText, _playerDefaultName , _logicRootsEmailDomain;
    static vector <string> _leagueNames, _medalNames, _starTypes, _praiseText, _levelText, _levelCongratulationText;
    static const string _currentLevelNameText, _levelNameText, _nullNameText;
    
    // Type: Int / Vector of int

    // Type: Double or vector<double>
    
    // Type: float or vector<float>

    static const float _animationFrameTime, _progressBarFillTime;
    
    // Type: Size
    static cocos2d::Size _designSize;
    
    // Type Color3B, Color4B etc.
    static Color3B _DarkBrown, _CountAnimationColor;
    // Color which will be used for setting the backGroud to give a mask effect for Popuops.
    static Color4B _PopUpMaskColor;
    
    // Structs:
    struct TypeName {
        string Res = "Res";
        string Power = "Power";
        string Task = "Task";
    };
    static TypeName _typeName;

    struct IndexName
    {
        string XIndex = "XIndex";
        string YIndex = "YIndex";
        string ZIndex = "ZIndex";
    };
    static IndexName _indexName;
    
    // Type: enum definitions here:
    enum Stage {
        // Loading screen:
        LoadingScreen,
        AppUpdateNotification,
        
        // HomeScreen:
        PlayGame,
        GameOver,
        GameWon,
        NewGameConfirmation,
        InitializeGameTypeChange,
        RateTheApp,
        InitializeDiffusersScreen,

        // FTUE
        FTUE,

        // Stage during processing/Asset loading:
        GameExitConfirmation,
        Unknown,
        NoInternet
    };
    
    enum SceneStage {
        LoadingScene = 1,
        HomeScene = 2,
        
        UnknownScene = 0
    };
    
    // Time of the day: Morning, Evening, Afternoon etc.
    enum DayTimeTransition {
        Sunrise = 1,
        Sunset = 2,
        
        UnknownDayTime = 100
    };
    
    enum Type {
        Resource = 1,
        Power = 2
    };

    enum MovementDirection {
        Up = 1,
        Right = 2,
        Down = 3,
        Left = 4
    };

    enum ActionTag {
        AutoTesting,
        Highlight
    };
    
    // Type: map
    static std::map<std::string, std::map<std::string, float>>
    // Practice Game Screen:
    _Game2048Xml;
    
    // Type: Vector or userDefined data types:
    std::vector<std::vector<std::string>>
    _game2048Assets = {
        //{ "MainBackground", "MainBackground", "MainBackground.png", "TOP" },
        { "GameTitleContainer", "GameTitleContainer", "GameTitleContainer.png", "TOP" },
        { "CurrentScoreContainer", "CurrentScoreContainer", "CurrentScoreContainer.png", "TOP" },
        { "BestScoreContainer", "CurrentScoreContainer", "BestScoreContainer.png", "TOP" },
        { "NewGameContainer", "RestartIcon", "RestartIcon.png", "TOP" },
        { "MainSquareContainer", "MainSquareContainer", "MainSquareContainer.png", "TOP" },
        //{ "GameModeBox", "GameModeNameAtom", "GameModeNameAtom.png", "LEFT" },
        { "ModeTitle", "ModeTitle", "ModeTitle.png", "LEFT" },
        { "TimerHolder", "TimerButton", "TimerButton.png", "TOP" },
        
        { "BombDiffuser", "BombDiffuser", "BombDiffuser.png", "TOP" },
        { "DiffuserContainer", "DiffuserContainer", "DiffuserContainer.png", "TOP" },
        //{ "RemoveAdsIcon", "RemoveAdsIcon", "RemoveAdsIcon.png", "TOP" },
        //{ "RestartIcon", "RestartIcon", "RestartIcon.png", "TOP" },
        //{ "ShareIcon", "ShareIcon", "ShareIcon.png", "TOP" },

        { "RateButton", "RemoveAdsButton", "RateButton.png", "TOP" },
        { "RemoveAdsButton", "RemoveAdsIcon", "RemoveAdsIcon.png", "TOP" },
        { "ShareButton", "ShareIcon", "ShareIcon.png", "TOP" },

        //{ "DiffusersDisplay", "DiffusersDisplay", "DiffusersDisplay.png", "TOP" }
    };
};

#endif // _DECLARATIONS_H_
