// Created by Radhey Shyam Meena
// March 2017.

/*
*********************************************************************
*****************************  ALGORITHM ****************************
*********************************************************************

    ********** STARTING THE GAME **********
    SetParameters: 
        Game Level, (will set gameDimensions)
        Difficulty level.
    
    Fill HUD data -> Following Data (from saved data/UserDefaults) For 
    above set Game level and difficulty level:
        Current Score. [key => "CurrentScore-LevelNum"]. Ex. "CurrentScore-4"
        Best Score. [key => "BestScore-LevelNum"]. Ex. "BestScore-4"

    Fill Tiles (SubSquareContainers) from saved data of that level:
        IF (currentScore > 0) - Since otherwise there is no point in trying
        to access saved value for each SubSquareContainer.
            Check if there is any data exits for a SubSquareContainer,
            Saving Format : key ==> "SubSquareContainerNumber-LevelNum:xIndex-yIndex".
            Ex. "SubSquareContainerNumber-4:0-3"
            if value > 0  --> Fill that SubSquareContainer with it's retrieved value and respective color.
            OtherWise don't create a SubSquareContainer.
        ELSE
            Fill two random SubSquareContainers.

    Set game Stage --> ReadyToPlay.

    Animations and Algorithms will take care of the operations.

    ***********  GAME OVER check *********
    IF (GameIsOver)
        Set key => "CurrentScore-LevelNum" -> Value to 0. [if there is no undo button in the game.]
        Delete key ==> "SubSquareContainerNumber-LevelNum:xIndex-yIndex". 
            Delete all saved SubSquareContainer values from memory. 


    ********** Restarting a game or Selecting New Game **********
                (of the same level)
    Set key => "CurrentScore-LevelNum" -> Value to 0.

    Start  "STARTING THE GAME" process.


    // Total pages:
    HomePage,
    ExitGame,
    RestartGame,
    GameWonContinueConfirmation,
    ModeChange,
    GameOver,
    RateTheApp.

*/

/* Image credit : 
bomb diffuser : https://play.google.com/store/apps/details?id=com.kanon.bombdefuser
*/
#include "Game2048Scene.h"
#include "Common.h"
#include "AdHelper.h"
#include "IAPHelper.h"
#include "SdkboxPlayHelper.h"
#include "Player.h"
#include "AnalyticsHelper.h"
#include "KeyNames.h"
#include "AppEvents.h"
#include "GameSound.h"
#include "GamerDevice.h"

USING_NS_CC;

Declarations Game2048::_declarations = Declarations();
Layer * Game2048::_game2048HomeLayer;
Layer * Game2048::_homeScreenLayer;
GameLevels * Game2048::_gameLevels;
int Game2048::_sceenTapCount = 0;
unsigned int Game2048::_inactiveSubContainerCounts = 0;
vector<int> Game2048::_inactiveNumbers = {};
bool Game2048::_isAutomatedTesting = false, Game2048::_isGameWon = false, Game2048::_isInactivatingOn = true, Game2048::_inactiveInstructionShown = false, Game2048::_isModeChangeActive = true;
Game2048Common::Vec2I Game2048::_subSquareContainerPossibleMovesCount;
float Game2048::_allTilesActiveTime = 60.0;
// Update : Tile enabled time increased from 45 secs to 60 secs,
//          since it was becoming difficult for users to score high.
// Ref : Leaderboard (GPG)

// Strings:
string Game2048::_disabledTileFTUETitle = "Atom Activated!", Game2048::_timerBoxFTUETitle = "Stay Focused!";

Scene* Game2048::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Game2048::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Game2048::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Game2048Common::_visibleSize = Director::getInstance()->getVisibleSize();
    Game2048::_isAutomatedTesting = false;
    // XT Layer touch enabling for swipe gesture:
    xtTouchEnabled(true);
    xtTouchMode(Touch::DispatchMode::ONE_BY_ONE);
    
    // For enabling keypad operation on the 'this' layer.
    this->setKeyboardEnabled(true);

    // Loading Plist:
    CommonFunc commonClassObj;

    // Loading plist data:
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Assets/Game2048.plist");

    // Loading XML:
    commonClassObj.loadAssetDetails("XML/Game20481X.xml", &Declarations::_Game2048Xml);

    _game2048HomeLayer = CommonFunc::createLayer(Vec2::ANCHOR_BOTTOM_LEFT, Vec2::ZERO, Game2048Common::_visibleSize);

    Game2048::_inactiveInstructionShown = UserDefault::getInstance()->getBoolForKey("InactiveInstructionShown", false);

    // Initializing game info:
    Game2048::SetGameInfo();

    Game2048::SetGameSettings(GameLevels::GameLevel::Four, GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo::Atom);

    // Test Section
    //Game2048::_gameLevels->SaveSubSquareContainerValues("0-0:2048-1-0;1-0:2048-1-0;2-0:64-1-0;3-0:8-1-0;");
    //("0-0:2-1-0;0-2:2-1-0;2-2:2-1-0");
    //// "0-0:2048-1-0;1-0:128-1-0;2-0:64-1-0;3-0:8-1-0;0-1:128-1-0;1-1:32-1-0;2-1:16-0-0;0-2:64-1-0;1-2:16-1-0;0-3:8-1-0;"
    //Game2048::_gameLevels->RetrieveSubSquareContainerValues();

    //Player::GetInstance()->SetDiffusersCount(5);

    // Test Section Ends

    // Consume IAP - Just to ensure that they are ready to be bought again.
    CommonFunc::ConsumeIAPProducts();

    // Starting a session:
    CommonFunc::StartSession();
    
    // Test Section
    //AnalyticsHelper::GetInstance()->SetTestMode(true);
    // Test Section Ends

    Game2048::InitializeHomeScreen();

    // Google Analytics:
    AnalyticsHelper::GetInstance()->EnableAdvertisingTracking(true);
    AnalyticsHelper::GetInstance()->CreateTracker(AnalyticsHelper::GetInstance()->GetTrackingId());

    // UserId set:
    AnalyticsHelper::GetInstance()->SetUser(CommonFunc::GetUUID());

    // Banner ad will be shown once it in loaded completely, hence adding a callback on ReceivedAd:
    // Showing the banner ad:
    function<void()> bannerShowCallback = []() {
        AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().BottomBanner);
    };
    AdHelper::GetInstance()->SetCallbackForAd(AdHelper::GetInstance()->GetAdNames().BottomBanner, bannerShowCallback);

    // User Sign in for leaderboards: 
    SdkboxPlayHelper::GetInstance()->SignIn();

    this->addChild(_game2048HomeLayer, 10);
    return true;
}

void Game2048::SetGameInfo()
{
    // Checking if it's first run of the game:
    int sessionCount = UserDefault::getInstance()->getIntegerForKey(KeyNames::GetInstance()->GetKeyName().SessionCount.c_str(), 0);
    if (sessionCount > 0) {
        sessionCount++;
    }
    else { // Starting first session:
        sessionCount = 1;
        UserDefault::getInstance()->setIntegerForKey(KeyNames::GetInstance()->GetKeyName().SessionCount.c_str(), sessionCount);

        // Giving user 2 diffusers at installation :
        Player::GetInstance()->SetDiffusersCount(3);

        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameInstallation, AppEvents::GetInstance()->GetEventAction().FirstRun, "First Run Of Game", 1);
    }
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().UserSession, AppEvents::GetInstance()->GetEventAction().SessionStarts, "User Session Starts", 1);
}

void Game2048::SetGameSettings(int gameLevel, GameLevels::GameDifficultyLevel difficultyLevel, GameLevels::GameAlgo gameAlgo) {
    // Setting Level Data:
    Game2048::_gameLevels = GameLevels::Create(GameLevels::GameLevel(gameLevel), difficultyLevel, gameAlgo);

    // Setting the _isInactivatingOn flag.
    string modeName = "NA", logActionModeStart = "NA", logLabelModeStart = "NA";
    switch (gameAlgo)
    {
    case GameLevels::Normal:
        modeName = Game2048::_gameLevels->_gameMode.Normal;
        logActionModeStart = AppEvents::GetInstance()->GetEventAction().NormalModeStarts;
        logLabelModeStart = modeName + " Mode Starts";
        Game2048::_isInactivatingOn = false;
        break;
    case GameLevels::OneInactive:
        Game2048::_isInactivatingOn = true;
        break;
    case GameLevels::Atom:
        // Event:
        modeName = Game2048::_gameLevels->_gameMode.Atom;
        logActionModeStart = AppEvents::GetInstance()->GetEventAction().AtomModeStarts;
        logLabelModeStart = modeName + " Mode Starts";
        Game2048::_isInactivatingOn = true;
        break;
    default:
        break;
    }

    log("SetGameSettings : Mode - %s, level - %d, difficulty - %d.", modeName.c_str(), gameLevel, difficultyLevel);
    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameModeStarts, logActionModeStart, logLabelModeStart, 1);

    if (Game2048::_gameLevels->GetPlayerCurrentScore() > 0)
    {
        Game2048::_gameLevels->RetrieveSubSquareContainerValues();
    }
    // Game data initialization:
}

void Game2048::ResetGameSetting() {
    Game2048::_gameLevels->~GameLevels();
    Game2048Common::CleanupLoadedData();
    Game2048::ResetInactiveNumbers();
    Game2048::_subSquareContainerPossibleMovesCount = Game2048Common::ZEROI;
    Game2048::_inactiveSubContainerCounts = 0;

    // Cleaning the tiles data saved.
    SubSquareContainer::CleanupAllSubSquareContaines();

    if (_homeScreenLayer)
    {
        _homeScreenLayer->removeFromParentAndCleanup(true);
    }
}

void Game2048::SetInactiveNumbers()
{
    if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Normal)
    {
        Game2048::_inactiveNumbers.clear();
    }
    else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::OneInactive)
    {
        int inactiveNumbersCountLimit = 1;

        for (int inactiveNumIndex = Game2048::_inactiveNumbers.size(); inactiveNumIndex < inactiveNumbersCountLimit; inactiveNumIndex++)
        {
            // Inactive Number :
            int numberRand = random(0, 30);
            int inactiveNumber = 0;
            if (numberRand <= 10) {
                inactiveNumber = 256;
            }
            else if (numberRand <= 20) {
                inactiveNumber = 512;
            }
            else {
                inactiveNumber = 1024;
            }

            Game2048::_inactiveNumbers.push_back(inactiveNumber);
        }
    }
    else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Atom)
    {
        int inactiveNumbersCountLimit = 1;

        for (int inactiveNumIndex = Game2048::_inactiveNumbers.size(); inactiveNumIndex < inactiveNumbersCountLimit; inactiveNumIndex++)
        {
            // Inactive Number:
            int numberRand = random(0, 40);
            int inactiveNumber = 0;
            if (numberRand <= 10) {
                inactiveNumber = 8;
            }
            else if (numberRand <= 20) {
                inactiveNumber = 16;
            }
            else if (numberRand <= 30) {
                inactiveNumber = 32;
            }
            //else if (numberRand <= 40) {
            //    inactiveNumber = 128;
            //}
            else {
                inactiveNumber = 64;
            }

            Game2048::_inactiveNumbers.push_back(inactiveNumber);
        }
    }
}

void Game2048::ResetInactiveNumbers()
{
    Game2048::_inactiveNumbers.clear();
}

void Game2048::InitializeHomeScreen(){
    _homeScreenLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, Vec2::ZERO, Game2048Common::_visibleSize, Color4B::WHITE, "HomeScreenLayer");
    CommonFunc::loadSpriteOnScreen(_homeScreenLayer, &_declarations._game2048Assets, &Declarations::_Game2048Xml);

    // Adding Labels:
    Sprite * gameTitleContainer = _homeScreenLayer->getChildByName<Sprite*>("GameTitleContainer");
    Label * gameTitleText = CommonFunc::createLabel("The Tile", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    gameTitleText->setDimensions(gameTitleContainer->getContentSize().width, gameTitleContainer->getContentSize().height);
    gameTitleContainer->addChild(gameTitleText, 5);

    Sprite * currentScoreContainer = _homeScreenLayer->getChildByName<Sprite*>("CurrentScoreContainer");
    Label * currentScoreContainerText = CommonFunc::createLabel("SCORE", Game2048Common::_gameTheme->GetFontFace().SemiBold, Game2048Common::_gameTheme->GetFontSize().Medium, currentScoreContainer->getContentSize().width/2.0, currentScoreContainer->getContentSize().height, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    currentScoreContainerText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    currentScoreContainerText->setName("CurrentScoreContainerText");
    currentScoreContainerText->setDimensions(currentScoreContainer->getContentSize().width, currentScoreContainer->getContentSize().height/2.0);
    currentScoreContainerText->enableWrap(false);

    Label * currentScoreContainerTextScore = CommonFunc::createLabel(CommonFunc::IntToString(Game2048::_gameLevels->GetPlayerCurrentScore()), Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, currentScoreContainer->getContentSize().width/2.0, 0.65 * currentScoreContainer->getContentSize().height/2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    currentScoreContainerTextScore->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    currentScoreContainerTextScore->setName("CurrentScoreContainerTextScore");
    currentScoreContainerTextScore->setDimensions(currentScoreContainer->getContentSize().width, 0.65 * currentScoreContainer->getContentSize().height);
    currentScoreContainerTextScore->enableWrap(false);

    currentScoreContainer->addChild(currentScoreContainerText, 5);
    currentScoreContainer->addChild(currentScoreContainerTextScore, 5);

    Sprite * bestScoreContainer = _homeScreenLayer->getChildByName<Sprite*>("BestScoreContainer");
    Label * bestScoreContainerText = CommonFunc::createLabel("HIGH SCORE", Game2048Common::_gameTheme->GetFontFace().SemiBold, Game2048Common::_gameTheme->GetFontSize().ExtraSmall, bestScoreContainer->getContentSize().width/2.0, bestScoreContainer->getContentSize().height, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    bestScoreContainerText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    bestScoreContainerText->setDimensions(bestScoreContainer->getContentSize().width, bestScoreContainer->getContentSize().height/2.0);
    bestScoreContainerText->enableWrap(false);
    bestScoreContainerText->setName("BestScoreContainerText");

    Label * bestScoreContainerTextScore = CommonFunc::createLabel(CommonFunc::IntToString(Game2048::_gameLevels->GetPlayerBestScore()), Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    bestScoreContainerTextScore->setDimensions(bestScoreContainer->getContentSize().width, 0.65 * bestScoreContainer->getContentSize().height);
    bestScoreContainerTextScore->enableWrap(false);
    bestScoreContainerTextScore->setName("BestScoreContainerTextScore");

    bestScoreContainer->addChild(bestScoreContainerText, 5);
    bestScoreContainer->addChild(bestScoreContainerTextScore, 5);

    bool isAtomMode = Game2048::_gameLevels->GetGameMode() == Game2048::_gameLevels->_gameMode.Atom;

    // Diffusers Container : To keep the count of diffusers available :
    auto diffuserContainer = _homeScreenLayer->getChildByName<Sprite*>("DiffuserContainer");
    diffuserContainer->setVisible(isAtomMode);

    Label * diffuserContainerText = CommonFunc::createLabel(CommonFunc::IntToString(Player::GetInstance()->GetDiffusersCount()), Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    diffuserContainerText->setName("DiffuserContainerText");
    diffuserContainerText->setDimensions(diffuserContainer->getContentSize().width, diffuserContainer->getContentSize().height);
    diffuserContainer->addChild(diffuserContainerText, 10);

    // Level Details: "| LEVEL | 4 | EASY |".
    string gameModeName = "GameModeNameAtom.png";// Color4B(Game2048Common::_gameTheme->GetGameColors().DiabledButton);
    if (Game2048::_isModeChangeActive)
    {
        if (Game2048::_gameLevels->GetGameMode() == Game2048::_gameLevels->_gameMode.Atom)
        {
            gameModeName = "GameModeNameAtom.png";
        }
        else {
            gameModeName = "GameModeNameNormal.png";
        }
    }
    else {
        gameModeName = "GameModeNameNormal.png";
    }

    auto levelTextHolderLayer = _homeScreenLayer->getChildByName<Sprite*>("ModeTitle");
    levelTextHolderLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    levelTextHolderLayer->setPosition(Vec2(gameTitleContainer->getBoundingBox().origin.x, diffuserContainer->getBoundingBox().origin.y));
    Label * levelText = CommonFunc::createLabel("MODE", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    levelText->setDimensions(levelTextHolderLayer->getContentSize().width, levelTextHolderLayer->getContentSize().height);

    // Difficulty Level:
    auto difficultyLevelHolderLayer = CommonFunc::createSpriteFromCache(gameModeName, levelTextHolderLayer->getContentSize().width + 3, 0, "GameModeBox");
    Label * difficultyLevelText = CommonFunc::createLabel(Game2048::_gameLevels->GetGameMode(), Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    difficultyLevelText->setDimensions(difficultyLevelHolderLayer->getContentSize().width, difficultyLevelHolderLayer->getContentSize().height);
    difficultyLevelHolderLayer->addChild(difficultyLevelText, 10);

    // Touch Listener to Change the GameType:
    auto listenerDifficultyLevelHolderLayer = cocos2d::EventListenerTouchOneByOne::create();
    listenerDifficultyLevelHolderLayer->setSwallowTouches(true);
    listenerDifficultyLevelHolderLayer->onTouchBegan = [difficultyLevelHolderLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::PlayGame) {
            if (CommonFunc::getWorldSpaceBoundingBox(difficultyLevelHolderLayer).containsPoint(touch->getLocation())) {
                if (!Game2048::_isAutomatedTesting)
                {
                    Game2048Common::_currentStage = Declarations::Stage::Unknown;
                }
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(difficultyLevelHolderLayer, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerDifficultyLevelHolderLayer->onTouchEnded = [difficultyLevelHolderLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(difficultyLevelHolderLayer, "ClosePopUpBoxButtonUnpressed.png");
        // The game mode change would not be possible via the buttom when autoTest is running.
        if (!Game2048::_isAutomatedTesting)
        {
            if (Game2048::_isModeChangeActive)
            {
                // Event:
                AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().ModeChange, "Game Mode Change Initiated", 1);

                Game2048::InitializeGameTypeChange();
            }
            else
            {
                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerDifficultyLevelHolderLayer, difficultyLevelHolderLayer);

    levelTextHolderLayer->addChild(levelText, 10);
    levelTextHolderLayer->addChild(difficultyLevelHolderLayer, 20);

    // Diffusers Container :
    auto diffusersDisplay = _homeScreenLayer->getChildByName<Sprite*>("BombDiffuser");
    diffusersDisplay->setVisible(isAtomMode);

    // Touch Listener to Change the GameType:
    auto listenerBombDiffuser = cocos2d::EventListenerTouchOneByOne::create();
    listenerBombDiffuser->setSwallowTouches(true);
    listenerBombDiffuser->onTouchBegan = [diffuserContainer, diffusersDisplay](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::PlayGame && diffusersDisplay->isVisible() && diffuserContainer->isVisible()) {
            if (CommonFunc::getWorldSpaceBoundingBox(diffusersDisplay).containsPoint(touch->getLocation()) || CommonFunc::getWorldSpaceBoundingBox(diffuserContainer).containsPoint(touch->getLocation())) {
                if (!Game2048::_isAutomatedTesting)
                {
                    Game2048Common::_currentStage = Declarations::Stage::Unknown;
                }
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(diffusersDisplay, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerBombDiffuser->onTouchEnded = [diffusersDisplay](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(diffusersDisplay, "ClosePopUpBoxButtonUnpressed.png");
        // The game mode change would not be possible via the buttom when autoTest is running.
        if (!Game2048::_isAutomatedTesting)
        {
                // Event:
                AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().AtomDiffusers, AppEvents::GetInstance()->GetEventAction().DiffuserScreen, "Diffusers Button Pressed", 1);

                Game2048::InitializeDiffusersScreen();
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerBombDiffuser, diffusersDisplay);

    Vec2 newGameContainerPosi = CommonFunc::getAlignRelativePosition("NewGameContainer.png", &Declarations::_Game2048Xml, "TOP");
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    /*
    Label * newGameContainerText = CommonFunc::createLabel("RESTART", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Small, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    newGameContainerText->setDimensions(newGameContainer->getContentSize().width, newGameContainer->getContentSize().height);
    newGameContainer->addChild(newGameContainerText, 5);
    */

    // New Game Button:
    auto listenerNewGameContainer = cocos2d::EventListenerTouchOneByOne::create();
    listenerNewGameContainer->setSwallowTouches(true);
    listenerNewGameContainer->onTouchBegan = [newGameContainer](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::PlayGame || Game2048Common::_currentStage == Declarations::Stage::GameWon) {
            if (CommonFunc::getWorldSpaceBoundingBox(newGameContainer).containsPoint(touch->getLocation())) {
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((CommonFunc::_soundPath + "ButtonClick.mp3").c_str());
                //CommonFunc::changeSpriteImage(newGameContainer, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerNewGameContainer->onTouchEnded = [newGameContainer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(newGameContainer, "ClosePopUpBoxButtonUnpressed.png");
        // If auto testing in ON
        if (Game2048::_isAutomatedTesting)
        {
            Game2048::StopAutomatedTesting();
        }
        else { // During the usual game play:
               // Event:
            AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().RestartGame, "Restart Initiated", 1);

            if (Game2048Common::_currentStage == Declarations::Stage::PlayGame)
            {
                Game2048::NewGameConfirmationPopup();
            }
            else if (Game2048Common::_currentStage == Declarations::Stage::GameWon)
            {
                auto gameWonLayer = Game2048::_game2048HomeLayer->getChildByName<Layer*>("GameWonLayer");
                if (gameWonLayer)
                {
                    gameWonLayer->runAction(RemoveSelf::create());
                    Game2048::NewGameConfirmationPopup();
                }
            }
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerNewGameContainer, newGameContainer);

    // Timer to show:
    Vec2 timerHolderPosition = Vec2(currentScoreContainer->getBoundingBox().origin.x, levelTextHolderLayer->getBoundingBox().origin.y);
    auto timerHolder = _homeScreenLayer->getChildByName("TimerHolder");
    timerHolder->setPosition(timerHolderPosition + timerHolder->getContentSize()/2.0);
    timerHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    timerHolder->setVisible(false);
    Label * timerText = CommonFunc::createLabel("00:00", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    timerText->setName("TimerText");
    timerText->setDimensions(timerHolder->getContentSize().width, timerHolder->getContentSize().height);
    timerHolder->addChild(timerText);

    // Touch Listener to the container:
    auto listenerTimerHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerTimerHolder->setSwallowTouches(false);
    listenerTimerHolder->onTouchBegan = [timerHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if ((Game2048Common::_currentStage == Declarations::Stage::PlayGame) || (Game2048Common::_currentStage == Declarations::Stage::FTUE)) {
            if (CommonFunc::getWorldSpaceBoundingBox(timerHolder).containsPoint(touch->getLocation()) && timerHolder->isVisible()) {
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                return true;
            }
        }
        return false;
    };
    listenerTimerHolder->onTouchEnded = [timerHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        Vec2 moveDelta = Vec2(abs(touch->getStartLocation().x - touch->getLocation().x), abs(touch->getStartLocation().x - touch->getLocation().x));
        float deltaXThreshold = 3.0;
        if (moveDelta.x <= deltaXThreshold && moveDelta.y <= deltaXThreshold)
        {
            Game2048::_inactiveInstructionShown = true;
            UserDefault::getInstance()->setBoolForKey("InactiveInstructionShown", Game2048::_inactiveInstructionShown);
            timerHolder->stopActionByTag(1);
            timerHolder->setScale(1.0);
            Game2048::SetInfoMessage(Game2048::GetNormalModeActiveMessage(Game2048::_allTilesActiveTime), timerHolder);
            //"All tiles stay unlocked when the timer is running. \nOnce the time has elapsed, one tile will lock randomly."
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerTimerHolder, timerHolder);

    // "MainSquareContainer":
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    int numOfSubSquareContainersToFill = 2;
    std::map<int, std::map<string, int>> subSquareContainersToFill;
    for (int subSquareContainerNum = 0; subSquareContainerNum < numOfSubSquareContainersToFill; subSquareContainerNum++){
        subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] = cocos2d::random(0, ((int)std::floor(Game2048::_gameLevels->GetGameDimensions().X) - 1));
        subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.YIndex] = cocos2d::random(0, ((int)floor(Game2048::_gameLevels->GetGameDimensions().Y) - 1));

        // Ensuring the the same container in not filled twice:
        //function<void()> selectUniqueSubSquareContainers = [subSquareContainersToFill, subSquareContainerNum]() {
        for (int subSquareContainerNumPrev = 0; subSquareContainerNumPrev < subSquareContainerNum; subSquareContainerNumPrev++)
        {
            if ((subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] == subSquareContainersToFill[subSquareContainerNumPrev][Declarations::_indexName.XIndex]) && (subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.YIndex] == subSquareContainersToFill[subSquareContainerNumPrev][Declarations::_indexName.YIndex])) 
            {
                if (subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] > 0)
                {
                    subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] = subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] - 1;
                }
                else
                {
                    subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] = subSquareContainersToFill[subSquareContainerNum][Declarations::_indexName.XIndex] + 1;
                }
            }
        }
        //};
    }

    // Resetting atom tiles :
    Game2048::_gameLevels->DisableAtomTiles();

    // Populating the existing tiles.
    for (int xIndex = 0; xIndex < Game2048::_gameLevels->GetGameDimensions().X; xIndex++) {
        for (int yIndex = 0; yIndex < Game2048::_gameLevels->GetGameDimensions().Y; yIndex++) {
            auto subSquareBaseContainer = CommonFunc::createSpriteFromCache("TileBase.png", 0, 0, "SubSquareBaseContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));

            subSquareBaseContainer->setPosition(Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerMargin + Vec2(xIndex * (Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerSize.width + Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerMargin.x), yIndex * (Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerSize.height + Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerMargin.y)));
            mainSquareContainer->addChild(subSquareBaseContainer,  1);

            // Check if any saved data is there for selected game and difficulty level:
            if (Game2048::_gameLevels->GetPlayerCurrentScore() > 0)
            {
                int subSquareContainerValue = Game2048::_gameLevels->GetSubSquareContainerValue(Game2048Common::Vec2I(xIndex, yIndex));
                if (subSquareContainerValue > 0)
                {
                    SubSquareContainer * subSquareContainer = Game2048::CreateSubSquareContainerWithValue(Game2048Common::Vec2I(xIndex, yIndex), subSquareContainerValue);
                    mainSquareContainer->addChild(subSquareContainer->GetTile(), subSquareBaseContainer->getLocalZOrder() + 5);
                    bool subSquareContainerIsActive = Game2048::_gameLevels->GetSubSquareContainerActive(Game2048Common::Vec2I(xIndex, yIndex));
                    subSquareContainer->SetSubSquareContainerActive(subSquareContainerIsActive);
                    if (!subSquareContainerIsActive) {
                        Game2048::_inactiveSubContainerCounts++;
                        Game2048::_inactiveNumbers.push_back(subSquareContainerValue);

                        Game2048::_gameLevels->SetAtomTile(subSquareContainer->GetSubSquareContainerActive(), xIndex, yIndex);
                    }

                    // Redundant
                    //subSquareContainer->SetSubSquareContainerActive(Game2048::_gameLevels->GetSubSquareContainerActive(Game2048Common::Vec2I(xIndex, yIndex)));
                    subSquareContainer->SetSubSquareContainerInactiveMovesCount(Game2048::_gameLevels->GetSubSquareContainerInactiveMovesCount(Game2048Common::Vec2I(xIndex, yIndex)));
                }
            }
            else
            {
                if (xIndex == subSquareContainersToFill[0][Declarations::_indexName.XIndex] && yIndex == subSquareContainersToFill[0][Declarations::_indexName.YIndex]) {
                    int subSquareContainerFillValue = CreateRandomValueForASubSquareContainer();
                    SubSquareContainer * subSquareContainer = Game2048::CreateSubSquareContainerWithValue(Game2048Common::Vec2I(xIndex, yIndex), subSquareContainerFillValue);
                    mainSquareContainer->addChild(subSquareContainer->GetTile(), subSquareBaseContainer->getLocalZOrder() + 5);
                }
                else if (xIndex == subSquareContainersToFill[1][Declarations::_indexName.XIndex] && yIndex == subSquareContainersToFill[1][Declarations::_indexName.YIndex]) {
                    int subSquareContainerFillValue = CreateRandomValueForASubSquareContainer();
                    SubSquareContainer * subSquareContainer = Game2048::CreateSubSquareContainerWithValue(Game2048Common::Vec2I(xIndex, yIndex), subSquareContainerFillValue);
                    mainSquareContainer->addChild(subSquareContainer->GetTile(), subSquareBaseContainer->getLocalZOrder() + 5);
                }
            }
        }
    }

    // Filling the remaining Inactive numbers:
    Game2048::SetInactiveNumbers();

    // Rate Us Button:
    // Redirect user to App Store.
    auto rateButton = _homeScreenLayer->getChildByName<Sprite*>("RateButton");
    //rateButton->setVisible(false);
    auto listenerRateButton = cocos2d::EventListenerTouchOneByOne::create();
    listenerRateButton->setSwallowTouches(true);
    listenerRateButton->onTouchBegan = [rateButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        if ((rateButton->isVisible()) &&(Game2048Common::_currentStage == Declarations::Stage::PlayGame)) {
            if (CommonFunc::getWorldSpaceBoundingBox(rateButton).containsPoint(touch->getLocation())) {
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(rateButton, "ClosePopUpBoxButtonPressed.png");
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                return true;
            }
        }
        return false;
    };

    listenerRateButton->onTouchEnded = [](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(rateButton, "ClosePopUpBoxButtonUnpressed.png");
        //CommonFunc::RedirectToAppStoreDetailsTab(Declarations::_AppID);
        
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().Leaderboard, "Leaderboard Pressed", 1);

        SdkboxPlayHelper::GetInstance()->ShowLeaderboard(SdkboxPlayHelper::GetInstance()->GetLeaderboardNames().HighestScore);
        Game2048Common::_currentStage = Declarations::Stage::PlayGame;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRateButton, rateButton);

    Label * rateButtonText = CommonFunc::createLabel("Leaderboard", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    rateButtonText->setDimensions(rateButton->getContentSize().width, rateButton->getContentSize().height);
    rateButton->addChild(rateButtonText);

    // "Remove Ads" button:
    auto removeAdsButton = _homeScreenLayer->getChildByName<Sprite*>("RemoveAdsButton");
    removeAdsButton->setVisible(!Player::GetInstance()->IsPaidFor(IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds));

    auto listenerRemoveAdsButton = cocos2d::EventListenerTouchOneByOne::create();
    listenerRemoveAdsButton->setSwallowTouches(true);
    listenerRemoveAdsButton->onTouchBegan = [removeAdsButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        if ((removeAdsButton->isVisible()) && (Game2048Common::_currentStage == Declarations::Stage::PlayGame)) {
            if (CommonFunc::getWorldSpaceBoundingBox(removeAdsButton).containsPoint(touch->getLocation())) {
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(removeAdsButton, "ClosePopUpBoxButtonPressed.png");
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                return true;
            }
        }
        return false;
    };

    listenerRemoveAdsButton->onTouchEnded = [removeAdsButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        // Call IAP "RemoveAds" when clicking on "Remove Ads" button.
        // on Success, Save that info with key "RemoveAds", it will be used to show/hide the "Remove Ads" button.
        // if "RemoveAds" = true --> hide the button.
        // else --> show the button

        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().RemoveAds, "Remove Ads Pressed", 1);

        //CommonFunc::changeSpriteImage(removeAdsButton, "ClosePopUpBoxButtonUnpressed.png");
        string iAPItemName = IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds;

        // If Success:
        function<void()> iAPSuccessCallback = [iAPItemName, removeAdsButton]() {
            // Hiding the remove Ads button.
            removeAdsButton->setVisible(false);

            // Hide the Bottom banner:
            AdHelper::GetInstance()->HideAd(AdHelper::GetInstance()->GetAdNames().BottomBanner);

            // Remove the ads from loading on screen.
            AdHelper::GetInstance()->RemoveListenerAds();

            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        };
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Success, iAPSuccessCallback);

        // If Failure or Canceled will do nothing since the game is already of the GameScreen activity.
        // Initiaing the Buying of IAP item.
        IAPHelper::GetInstance()->BuyIAPItem(iAPItemName, 0);

        Game2048Common::_currentStage = Declarations::Stage::PlayGame;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerRemoveAdsButton, removeAdsButton);

    /*
    Label * removeAdsButtonText = CommonFunc::createLabel("Remove Ads", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    removeAdsButtonText->setDimensions(removeAdsButton->getContentSize().width, removeAdsButton->getContentSize().height);
    removeAdsButton->addChild(removeAdsButtonText);
    */

    // Share Button:
    // Fire the share options.
    auto shareButton = _homeScreenLayer->getChildByName<Sprite*>("ShareButton");

    auto listenerShareButton = cocos2d::EventListenerTouchOneByOne::create();
    listenerShareButton->setSwallowTouches(true);
    listenerShareButton->onTouchBegan = [shareButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (shareButton->isVisible() && (Game2048Common::_currentStage == Declarations::Stage::PlayGame)) {
            if (CommonFunc::getWorldSpaceBoundingBox(shareButton).containsPoint(touch->getLocation())) {
                // Register Event:
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(shareButton, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerShareButton->onTouchEnded = [shareButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(shareButton, "ClosePopUpBoxButtonUnpressed.png");
        // TBD - better the structure. UpdateTotalSocialMediaShares need not be called alongside each 
        // CallShare() function call. It should be auto recorded when sharing is triggered.
        Player::GetInstance()->UpdateTotalSocialMediaShares();
        CommonFunc::CallShare();
        // Share button clicked:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().LinkSharing, "Share Button Pressed", 1);

        Game2048Common::_currentStage = Declarations::Stage::PlayGame;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerShareButton, shareButton);

    // Add share Icon to share button:
    /* Below code commented because a new share icon integrated with, that doesn't need a separate icon on it or a text */
    /*
    auto shareIcon = CommonFunc::createSpriteFromCache("ShareIcon.png", 35, shareButton->getContentSize().height / 2.0, "ShareIcon");
    shareIcon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    */

    // Add "Share" text on the share button:
    /*
    Label * shareButtonText = CommonFunc::createLabel("Share", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 60, 0, Color3B::WHITE, cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment::CENTER);
    shareButtonText->setDimensions(shareButton->getContentSize().width - shareButtonText->getPosition().x, shareButton->getContentSize().height);

    shareButton->addChild(shareIcon);
    shareButton->addChild(shareButtonText); 
    */

    // Signature : email-ID at the bottom of Screen:
    Label * signatureText = CommonFunc::createLabel("info.developer2017@gmail.com", Game2048Common::_gameTheme->GetFontFace().SemiBold, Game2048Common::_gameTheme->GetFontSize().Medium, 3, 3, Game2048Common::_gameTheme->GetGameColors().TextColorDarkBrown, cocos2d::TextHAlignment::RIGHT, cocos2d::TextVAlignment::BOTTOM);
    signatureText->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    signatureText->setWidth(Game2048Common::_visibleSize.width - 6);
    signatureText->setVisible(false);
    _homeScreenLayer->addChild(signatureText, 10);

    _game2048HomeLayer->addChild(_homeScreenLayer, 10);
    Game2048Common::_currentStage = Declarations::Stage::PlayGame;

    // Google Analytics:
    AnalyticsHelper::GetInstance()->LogScreen("GameScene");
    AnalyticsHelper::GetInstance()->LogEvent("GameStarts", "HomeScreenCreated", "Home Screen loads", 1);

    if (Game2048::_isAutomatedTesting) {
        Game2048::CountPossibleMoves();
        Game2048::StartAutomatedTesting();
    }
}

void Game2048::InitializeGameTypeChange() {
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    Layer * initializeGameTypeChangeLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "InitializeGameTypeChangeLayer");

    // Game Over text:
    Label * initializeGameTypeChangeText = CommonFunc::createLabel("Change Game Mode", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, initializeGameTypeChangeLayer->getContentSize().width / 2.0, initializeGameTypeChangeLayer->getContentSize().height / 2.0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    initializeGameTypeChangeText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    initializeGameTypeChangeText->setDimensions(initializeGameTypeChangeLayer->getContentSize().width, initializeGameTypeChangeLayer->getContentSize().height / 4.0);

    // "Normal" mode button:
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    auto normalModeHolder = CommonFunc::createSpriteFromCache("NormalModeButton.png", initializeGameTypeChangeLayer->getContentSize().width / 2.0, initializeGameTypeChangeLayer->getContentSize().height / 2.0, "NormalModeHolder");
    normalModeHolder->setPosition(normalModeHolder->getPositionX(), normalModeHolder->getPositionY() - 1.5 * normalModeHolder->getContentSize().height);
    normalModeHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);

    auto listenerNormalModeHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerNormalModeHolder->setSwallowTouches(true);
    listenerNormalModeHolder->onTouchBegan = [normalModeHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::InitializeGameTypeChange) {
            if (CommonFunc::getWorldSpaceBoundingBox(normalModeHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(normalModeHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerNormalModeHolder->onTouchEnded = [initializeGameTypeChangeLayer, normalModeHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(normalModeHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameModeChange, AppEvents::GetInstance()->GetEventAction().NormalModeInitiated, "Normal Mode Initiated", 1);

        // IAP to pay for playing the Normal mode: item "UnlockNormalMode".
        // If the payment succeeds, then record that payment locally with key "UnlockNormalMode",
        // and proceed to play normal mode.
        // Otherwise don't do anything.
        string iAPItemName = IAPHelper::GetInstance()->GetIAPItemNames().UnlockNormalMode;

        bool isPaid = Player::GetInstance()->IsPaidFor(iAPItemName);
        if (isPaid)
        {
            log("listenerNormalModeHolder: isPaid = %d, for %s.", isPaid, iAPItemName.c_str());
            initializeGameTypeChangeLayer->runAction(Sequence::create(CallFunc::create([]() {
                Game2048::ResetGameSetting();
                Game2048::SetGameSettings(Game2048::_gameLevels->GetGameLevel(), GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo::Normal);

                Game2048::InitializeHomeScreen();
            }), RemoveSelf::create(), NULL));
        }
        else {
            // If Success:
            function<void()> iAPSuccessCallback = [iAPItemName, initializeGameTypeChangeLayer]() {
                // and then start the Normal mode :
                log("listenerNormalModeHolder: iAPSuccessCallback for %s.", iAPItemName.c_str());
                initializeGameTypeChangeLayer->runAction(Sequence::create(CallFunc::create([]() {
                    Game2048::ResetGameSetting();
                    Game2048::SetGameSettings(Game2048::_gameLevels->GetGameLevel(), GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo::Normal);

                    Game2048::InitializeHomeScreen();
                }), RemoveSelf::create(), NULL));
            };
            IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Success, iAPSuccessCallback);

            // If Failure or Canceled : Get back to Atom Mode game play:
            function<void()> iAPFailureCallback = [iAPItemName, initializeGameTypeChangeLayer]() {
                // and then start the Normal mode :
                log("listenerNormalModeHolder: iAPFailureCallback for %s.", iAPItemName.c_str());
                initializeGameTypeChangeLayer->runAction(Sequence::create(CallFunc::create([]() {
                    Game2048Common::_currentStage = Declarations::Stage::PlayGame;
                }), RemoveSelf::create(), NULL));
            };
            IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Failure, iAPFailureCallback);
            IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Canceled, iAPFailureCallback);

            // Initiaing the Buying of IAP item.
            IAPHelper::GetInstance()->BuyIAPItem(iAPItemName, 0);

            Game2048Common::_currentStage = Declarations::Stage::InitializeGameTypeChange;
        }
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerNormalModeHolder, normalModeHolder);

    Label * normalModeText = CommonFunc::createLabel("NORMAL", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    normalModeText->setDimensions(normalModeHolder->getContentSize().width, normalModeHolder->getContentSize().height);
    normalModeHolder->addChild(normalModeText, 10);

    // ATOM MODE SELECTION:
    auto atomModeHolder = CommonFunc::createSpriteFromCache("AtomModeButton.png", initializeGameTypeChangeLayer->getContentSize().width / 2.0, initializeGameTypeChangeLayer->getContentSize().height / 2.0, "AtomModeHolder");
    atomModeHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    auto listenerAtomModeHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerAtomModeHolder->setSwallowTouches(true);
    listenerAtomModeHolder->onTouchBegan = [atomModeHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::InitializeGameTypeChange) {
            if (CommonFunc::getWorldSpaceBoundingBox(atomModeHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(atomModeHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerAtomModeHolder->onTouchEnded = [initializeGameTypeChangeLayer, atomModeHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(atomModeHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameModeChange, AppEvents::GetInstance()->GetEventAction().AtomModeInitiated, "Atom Mode Initiated", 1);

        initializeGameTypeChangeLayer->runAction(Sequence::create(CallFunc::create([]() {
            Game2048::ResetGameSetting();
            Game2048::SetGameSettings(Game2048::_gameLevels->GetGameLevel(), GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo::Atom);

            Game2048::InitializeHomeScreen();
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerAtomModeHolder, atomModeHolder);

    Label * atomModeText = CommonFunc::createLabel("ATOM", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    //atomModeText->enableGlow(Color4B::YELLOW);
    atomModeText->enableGlow(Color4B::BLUE);
    atomModeText->setDimensions(atomModeHolder->getContentSize().width, atomModeHolder->getContentSize().height);
    atomModeHolder->addChild(atomModeText, 10);

    initializeGameTypeChangeLayer->addChild(atomModeHolder, 10);
    initializeGameTypeChangeLayer->addChild(normalModeHolder, 10);
    initializeGameTypeChangeLayer->addChild(initializeGameTypeChangeText, 10);
    _game2048HomeLayer->addChild(initializeGameTypeChangeLayer, 100);
    initializeGameTypeChangeLayer->setOpacity(0);
    initializeGameTypeChangeLayer->runAction(Sequence::create(FadeTo::create(0.5, 200), CallFunc::create([]() {
    }), NULL));
    Game2048Common::_currentStage = Declarations::Stage::InitializeGameTypeChange;
}

void Game2048::InitializeDiffusersScreen()
{
    //  : Diffuser display page : having
    // one button to utilize a diffuser
    // one to buy 5 diffusers 
    // and one to buy 40 diffusers.

    // Consume BuyYDiffusers IAP - Just to ensure that it is ready to be bought again.
    // TBD : verify the consumption.
    CommonFunc::ConsumeIAPProducts();

    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    Layer * initializeDiffusersScreenLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "InitializeDiffusersScreenLayer");

    // Game Over text:
    Label * initializeDiffusersScreenText = CommonFunc::createLabel("Atom Diffusers", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, initializeDiffusersScreenLayer->getContentSize().width / 2.0, 0.72 * initializeDiffusersScreenLayer->getContentSize().height, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    initializeDiffusersScreenText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    initializeDiffusersScreenText->setDimensions(initializeDiffusersScreenLayer->getContentSize().width, 0.28 * initializeDiffusersScreenLayer->getContentSize().height);

    // Utilize the diffusers button :
    auto diffuseAtomButton = CommonFunc::createSpriteFromCache("DiffuseAtomButtom.png", initializeDiffusersScreenLayer->getContentSize().width/2.0, 411, "DiffuseAtomButton");
    diffuseAtomButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    
    // "Buy Diffuser Now" text on uses button when there are no diffusers available:
    Label * diffuseAtomButtonText = CommonFunc::createLabel("Diffuse Atom", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, cocos2d::Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    diffuseAtomButtonText->setDimensions(diffuseAtomButton->getContentSize().width, diffuseAtomButton->getContentSize().height);
    diffuseAtomButton->addChild(diffuseAtomButtonText, 10);

    map<int, map<int, int>> atomTiles = Game2048::_gameLevels->GetAtomTiles();

    // 
    //log("InitializeDiffusersScreen, Diffuser count : %d", Player::GetInstance()->GetDiffusersCount());

    if (Player::GetInstance()->GetDiffusersCount() <= 0) {
        // When there are no diffusers available:
        log("InitializeDiffusersScreen, Diffuser count less than or equal to 0 : %d", Player::GetInstance()->GetDiffusersCount());
        CommonFunc::changeSpriteImage(diffuseAtomButton, "DiffuseAtomButtonDisabled.png");
        diffuseAtomButtonText->setString("No Diffusers");
    } else if (atomTiles.begin() == atomTiles.end()) {
        log("InitializeDiffusersScreen, No atom tiles : %d", Player::GetInstance()->GetDiffusersCount());
        CommonFunc::changeSpriteImage(diffuseAtomButton, "DiffuseAtomButtonDisabled.png");
    }

    log("InitializeDiffusersScreen, No atom tiles : %d", Player::GetInstance()->GetDiffusersCount());

    // Add click clisterner :
    auto listenerDiffuseAtomButton = cocos2d::EventListenerTouchOneByOne::create();
    listenerDiffuseAtomButton->setSwallowTouches(true);
    listenerDiffuseAtomButton->onTouchBegan = [diffuseAtomButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::InitializeDiffusersScreen) {
            if (CommonFunc::getWorldSpaceBoundingBox(diffuseAtomButton).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;

                map<int, map<int, int>> atomTiles = Game2048::_gameLevels->GetAtomTiles();

                if (Player::GetInstance()->GetDiffusersCount() > 0 && atomTiles.begin() != atomTiles.end()) {
                    // There are diffusers in your bucket to use:
                    GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().AtomDiffusers, AppEvents::GetInstance()->GetEventAction().DiffuserUtilization, "Atom Diffused", 1);
                    return true;
                }
                else {
                    // There are no diffusers with you, you need to buy some to diffuse atom:
                    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().AtomDiffusers, AppEvents::GetInstance()->GetEventAction().DiffuserUtilization, "Diffuse Atom Denied", 1);
                    GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().DisabledButtonClick);
                    GamerDevice::GetInstance()->VibrateDevice(GamerDevice::GetInstance()->GetVibrateType().Short);
                    Game2048Common::_currentStage = Declarations::Stage::InitializeDiffusersScreen;
                }
                //CommonFunc::changeSpriteImage(diffuseAtomButton, "ClosePopUpBoxButtonPressed.png");
            }
        }
        return false;
    };

    listenerDiffuseAtomButton->onTouchEnded = [initializeDiffusersScreenLayer, diffuseAtomButton](cocos2d::Touch* touch, cocos2d::Event* event) {
        // Atom diffused by the user :
        // Actions
        // - Make the DiffuserScreen disappear .
        // - Reduce the diffuser count.
        // - Play "AtomDiffused" sound.
        // - Play "AtomDiffused" animation.
        // - Diffuse the atom on tile - Change its color to normal and its active status accordingly.
        initializeDiffusersScreenLayer->runAction(Sequence::create(CallFunc::create([]() {
            Player::GetInstance()->UpdateDiffusersCountBy(-1);
            GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ShortCelebration);
            // Fetch the disabled tile (need to save that info) / Atom.
            map<int, map<int, int>> atomTiles = Game2048::_gameLevels->GetAtomTiles();
            map<int, map<int, int>>::iterator atomIterator = atomTiles.begin();
            SubSquareContainer * atomTile = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(atomIterator->first) + "-" + CommonFunc::IntToString(atomIterator->second.begin()->first));

            //atomTile->GetTile()->ignoreAnchorPointForPosition(true);
            //atomTile->GetTile()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            FiniteTimeAction * atomDiffuseAnimation = Sequence::create(RotateBy::create(0.5, Vec3(90, 0, 0)), CallFunc::create([atomTile]() {
                Game2048::SetAtomActive(atomTile, false);
                // Update hud as well.
                Game2048::UpdateHUD();
            }), RotateBy::create(0.5, Vec3(-90, 0, 0)), NULL);

            atomTile->GetTile()->runAction(Sequence::create(atomDiffuseAnimation, CallFunc::create([atomTile]() {
                //  Set it's status to active:
                //atomTile->SetSubSquareContainerActive(true);
            }), NULL));
        }), RemoveSelf::create(), NULL));

        Game2048Common::_currentStage = Declarations::Stage::PlayGame;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerDiffuseAtomButton, diffuseAtomButton);

    // "Buy X diffusers" button:
    auto buyXDiffusersHolder = CommonFunc::createSpriteFromCache("Buy5UnitsContainer.png", initializeDiffusersScreenLayer->getContentSize().width / 2.0, 237, "BuyXDiffusersHolder");
    buyXDiffusersHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);

    auto listenerBuyXDiffusersHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerBuyXDiffusersHolder->setSwallowTouches(true);
    listenerBuyXDiffusersHolder->onTouchBegan = [buyXDiffusersHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::InitializeDiffusersScreen) {
            if (CommonFunc::getWorldSpaceBoundingBox(buyXDiffusersHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(buyXDiffusersHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerBuyXDiffusersHolder->onTouchEnded = [initializeDiffusersScreenLayer, buyXDiffusersHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(buyXDiffusersHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().AtomDiffusers, AppEvents::GetInstance()->GetEventAction().DiffuserBuy, "Diffuser Small Buy Initiated", 1);

        // IAP to pay for playing the Normal mode: item "UnlockBuyXDiffusers".
        // If the payment succeeds, then record that payment locally with key "UnlockBuyXDiffusers",
        // and proceed to play normal mode.
        // Otherwise don't do anything.
        string iAPItemName = IAPHelper::GetInstance()->GetIAPItemNames().BuyXDiffusers;

        // If Success:
        function<void()> iAPSuccessCallback = [iAPItemName, initializeDiffusersScreenLayer]() {
            // and then start the Normal mode :
            log("listenerBuyXDiffusersHolder: iAPSuccessCallback for %s.", iAPItemName.c_str());
            initializeDiffusersScreenLayer->runAction(Sequence::create(CallFunc::create([]() {
                // Broadcast the diffusers information to homescreen. This updates the diffusers count on HUD.
                Player::GetInstance()->UpdateDiffusersCountBy(5);
                Game2048::UpdateHUD();
                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }), RemoveSelf::create(), NULL));
        };
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Success, iAPSuccessCallback);

        // If Failure or Canceled : Get back to Atom Mode game play:
        function<void()> iAPFailureCallback = [iAPItemName, initializeDiffusersScreenLayer]() {
            // and then start the Normal mode :
            log("listenerBuyXDiffusersHolder: iAPFailureCallback for %s.", iAPItemName.c_str());
            initializeDiffusersScreenLayer->runAction(Sequence::create(CallFunc::create([]() {
                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }), RemoveSelf::create(), NULL));
        };
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Failure, iAPFailureCallback);
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Canceled, iAPFailureCallback);

        // Initiaing the Buying of IAP item.
        IAPHelper::GetInstance()->BuyIAPItem(iAPItemName, 0);

        Game2048Common::_currentStage = Declarations::Stage::InitializeDiffusersScreen;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerBuyXDiffusersHolder, buyXDiffusersHolder);

    Label * buyXDiffusersText = CommonFunc::createLabel("Buy 5 Diffusers", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    buyXDiffusersText->setDimensions(buyXDiffusersHolder->getContentSize().width, buyXDiffusersHolder->getContentSize().height);
    buyXDiffusersHolder->addChild(buyXDiffusersText, 10);

    // Buy Y diffusers SELECTION:
    auto buyYDiffusersHolder = CommonFunc::createSpriteFromCache("Buy40UnitsContainer.png", initializeDiffusersScreenLayer->getContentSize().width / 2.0, 122, "BuyYDiffusersHolder");
    buyYDiffusersHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    auto listenerBuyYDiffusersHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerBuyYDiffusersHolder->setSwallowTouches(true);
    listenerBuyYDiffusersHolder->onTouchBegan = [buyYDiffusersHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::InitializeDiffusersScreen) {
            if (CommonFunc::getWorldSpaceBoundingBox(buyYDiffusersHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(buyYDiffusersHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerBuyYDiffusersHolder->onTouchEnded = [initializeDiffusersScreenLayer, buyYDiffusersHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(buyYDiffusersHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().AtomDiffusers, AppEvents::GetInstance()->GetEventAction().DiffuserBuy, "Diffuser Large Buy Initiated", 1);

        // Verify following :
        string iAPItemName = IAPHelper::GetInstance()->GetIAPItemNames().BuyYDiffusers;

        // If Success:
        function<void()> iAPSuccessCallback = [iAPItemName, initializeDiffusersScreenLayer]() {
            // and then start the Normal mode :
            log("listenerBuyYDiffusersHolder: iAPSuccessCallback for %s.", iAPItemName.c_str());
            initializeDiffusersScreenLayer->runAction(Sequence::create(CallFunc::create([]() {
                // Broadcast the diffusers information to homescreen. This updates the diffusers count.
                Player::GetInstance()->UpdateDiffusersCountBy(40);
                Game2048::UpdateHUD();
                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }), RemoveSelf::create(), NULL));
        };
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Success, iAPSuccessCallback);

        // If Failure or Canceled : Get back to Atom Mode game play:
        function<void()> iAPFailureCallback = [iAPItemName, initializeDiffusersScreenLayer]() {
            // and then start the Normal mode :
            log("listenerBuyYDiffusersHolder: iAPFailureCallback for %s.", iAPItemName.c_str());
            initializeDiffusersScreenLayer->runAction(Sequence::create(CallFunc::create([]() {
                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }), RemoveSelf::create(), NULL));
        };
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Failure, iAPFailureCallback);
        IAPHelper::GetInstance()->SetCallbackForIAPItem(iAPItemName, IAPHelper::IAPOutcome::Canceled, iAPFailureCallback);

        // Initiaing the Buying of IAP item.
        IAPHelper::GetInstance()->BuyIAPItem(iAPItemName, 0);

        Game2048Common::_currentStage = Declarations::Stage::InitializeDiffusersScreen;
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerBuyYDiffusersHolder, buyYDiffusersHolder);

    Label * buyYDiffusersText = CommonFunc::createLabel("Buy 40 Diffusers", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    //buyYDiffusersText->enableGlow(Color4B::YELLOW);
    buyYDiffusersText->enableGlow(Color4B::BLUE);
    buyYDiffusersText->setDimensions(buyYDiffusersHolder->getContentSize().width, buyYDiffusersHolder->getContentSize().height);
    buyYDiffusersHolder->addChild(buyYDiffusersText, 10);

    initializeDiffusersScreenLayer->addChild(diffuseAtomButton, 10);
    initializeDiffusersScreenLayer->addChild(buyYDiffusersHolder, 10);
    initializeDiffusersScreenLayer->addChild(buyXDiffusersHolder, 10);
    initializeDiffusersScreenLayer->addChild(initializeDiffusersScreenText, 10);
    _game2048HomeLayer->addChild(initializeDiffusersScreenLayer, 100);
    initializeDiffusersScreenLayer->setOpacity(0);
    initializeDiffusersScreenLayer->runAction(Sequence::create(FadeTo::create(0.5, 200), CallFunc::create([]() {
    }), NULL));
    Game2048Common::_currentStage = Declarations::Stage::InitializeDiffusersScreen;
}

void Game2048::SetAtomActive(SubSquareContainer * subSquareContainer, bool isActive)
{
    if (isActive) {
        // Atom enables :
    }
    else {
        // Atom disables :
        // Resetting atom tiles :
        Game2048::_gameLevels->DisableAtomTiles();

        Game2048::CleanupInfoText();
        subSquareContainer->GetTile()->stopActionByTag(1);
        subSquareContainer->GetTile()->setScale(1.0);
        subSquareContainer->SetSubSquareContainerActive(true);
        subSquareContainer->SetSubSquareContainerInactiveMovesCount(0);
        Game2048::_inactiveSubContainerCounts = MAX(0, Game2048::_inactiveSubContainerCounts - 1);
        Game2048::ResetInactiveNumbers();
        Game2048::SetInactiveNumbers();

        Game2048::_isInactivatingOn = false;
        Game2048::StartTimer(Game2048::_allTilesActiveTime, []() {
            Game2048::_isInactivatingOn = true;
        });

        // FTUE:
        if (!Game2048::_inactiveInstructionShown)
        {
            auto timerHolder = _homeScreenLayer->getChildByName("TimerHolder");
            Game2048::StartFTUE(Game2048::_timerBoxFTUETitle, timerHolder);
        }
    }
}

void Game2048::CleanupInfoText()
{
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    Label * infoText = mainSquareContainer->getChildByName<Label*>("InfoText");
    if (infoText)
    {
        infoText->removeFromParentAndCleanup(true);

    }
}

void Game2048::StartFTUE(string titleText, Node * nodeFTUE)
{
    //Game2048Common::_currentStage = Declarations::Stage::FTUE;
    // This will set the titleText in the FTUE infoBox (stage is set to FTUE) and 
    // when the nodeFTUE is clicked, it will display the FTUEbody text in this box.
    // Which remains there for some time and disappears:

    // "MainSquareContainer":
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    Label * infoText = mainSquareContainer->getChildByName<Label*>("InfoText");
    if (infoText)
    {
        infoText->removeFromParentAndCleanup(true);
    }
    infoText = CommonFunc::createLabel(titleText, Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, mainSquareContainer->getContentSize().width / 2.0, mainSquareContainer->getContentSize().height + 10, Color3B::BLACK, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::BOTTOM);
    infoText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    infoText->setName("InfoText");
    infoText->setLocalZOrder(1000);
    infoText->setAdditionalKerning(-1.0);
    infoText->setDimensions(mainSquareContainer->getContentSize().width, mainSquareContainer->getContentSize().height / (1.0 * Game2048::_gameLevels->GetGameDimensions().X));
    infoText->enableWrap(false);
    mainSquareContainer->addChild(infoText);

    infoText->runAction(Sequence::create(Show::create(), RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.05), ScaleTo::create(0.5, 0.95), NULL)), NULL));
}

void Game2048::SetInfoMessage(string infoTextMessage, Node * node)
{
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    Label * infoText = mainSquareContainer->getChildByName<Label*>("InfoText");
    if (infoText)
    {
        infoText->removeFromParentAndCleanup(true);
    }
    infoText = CommonFunc::createLabel(infoTextMessage, Game2048Common::_gameTheme->GetFontFace().SemiBold, Game2048Common::_gameTheme->GetFontSize().Medium, 0.0, mainSquareContainer->getContentSize().height + 10, Color3B::BLACK, cocos2d::TextHAlignment::CENTER, TextVAlignment::BOTTOM);
    infoText->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    infoText->setName("InfoText");
    infoText->enableWrap(false);
    infoText->setScale(0.0);
    infoText->setDimensions(mainSquareContainer->getContentSize().width, mainSquareContainer->getContentSize().height / (1.0 * Game2048::_gameLevels->GetGameDimensions().X));
    infoText->setLocalZOrder(1000);
    infoText->setAdditionalKerning(-1.0);
    mainSquareContainer->addChild(infoText);

    Vec2 positionDelta = mainSquareContainer->convertToNodeSpace(CommonFunc::getWorldSpaceBoundingBox(node).origin) + node->getContentSize() / 2.0 - infoText->getPosition();
    infoText->setAnchorPoint(Vec2(positionDelta.x / infoText->getWidth(), positionDelta.y / infoText->getHeight()));
    infoText->setPosition(mainSquareContainer->convertToNodeSpace(CommonFunc::getWorldSpaceBoundingBox(node).origin) + node->getContentSize() / 2.0);

    infoText->runAction(Sequence::create(ScaleTo::create(0.500, 1.0), DelayTime::create(15.0), RemoveSelf::create(), NULL));
}

string Game2048::GetNormalModeActiveMessage(float time) {
    return "Atom remains diffused for " + CommonFunc::IntToString(time) + " secs.\nScore quickly to gain advantage.";
}

inline void Game2048::StartTimer(float totalTime, function<void()> callback)
{
    auto timerHolder = _homeScreenLayer->getChildByName("TimerHolder");
    auto timerText = timerHolder->getChildByName<Label*>("TimerText");
    timerText->setString(Game2048Common::SecondsToTimeStamp((int)(ceil(totalTime)), 2));

    if (!Game2048::_inactiveInstructionShown)
    {
        FiniteTimeAction * scaleAnimation = RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.2), ScaleTo::create(0.5, 0.8), NULL));
        scaleAnimation->setTag(1);
        timerHolder->runAction(scaleAnimation);
    }

    // Fire the timer explanation message when timer starts.
    Game2048::SetInfoMessage(Game2048::GetNormalModeActiveMessage(Game2048::_allTilesActiveTime), timerHolder);

    timerHolder->runAction(Sequence::create(Show::create(), Repeat::create(Sequence::create(CallFunc::create([totalTime, timerText]() mutable {
        if (totalTime > 0.0)
        {
            timerText->setString(Game2048Common::SecondsToTimeStamp((int)(ceil(totalTime)), 2));
        }
        else {
            timerText->setString(Game2048Common::SecondsToTimeStamp(0, 2));
        }
        totalTime--;
    }), DelayTime::create(1.0), NULL), (int)(ceil(totalTime))), CallFunc::create([callback]() {
        Game2048::CleanupInfoText();
        callback();
    }), Hide::create(), NULL));
}

void Game2048::StopTimer()
{
    auto timerHolder = _homeScreenLayer->getChildByName("TimerHolder");
    timerHolder->stopAllActions();
    timerHolder->setVisible(false);
}

int Game2048::CreateRandomValueForASubSquareContainer() {
    int numToSpawn = 2;
    if (Game2048::_gameLevels->GetDifficultyLevel() == GameLevels::GameDifficultyLevel::Easy) {
        numToSpawn = (random(0, 11) <= 9) ? 2 : 4;
    }
    else if (Game2048::_gameLevels->GetDifficultyLevel() == GameLevels::GameDifficultyLevel::Medium) {
        numToSpawn = (random(0, 11) <= 5) ? 2 : 4;
    }
    else if (Game2048::_gameLevels->GetDifficultyLevel() == GameLevels::GameDifficultyLevel::Hard) {
        int randNum = random(0, 11);
        numToSpawn = (randNum <= 3) ? 2 : ((randNum <= 7) ? 4 : 8);
    }

    return numToSpawn;
}

SubSquareContainer * Game2048::CreateSubSquareContainerWithValue(Game2048Common::Vec2I containerIndex, int value) {
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    auto subSquareBaseContainer = mainSquareContainer->getChildByName<Sprite*>("SubSquareBaseContainer" + CommonFunc::IntToString(containerIndex.X) + "-" + CommonFunc::IntToString(containerIndex.Y));
    if (subSquareBaseContainer) {
        SubSquareContainer * subSquareContainer = SubSquareContainer::CreateSprite(SubSquareContainer::GetSpriteNameForNumber(value), mainSquareContainer->convertToNodeSpace(CommonFunc::getWorldSpaceBoundingBox(subSquareBaseContainer).origin), containerIndex, "SubSquareContainer" + CommonFunc::IntToString(containerIndex.X) + "-" + CommonFunc::IntToString(containerIndex.Y));
        /*SubSquareContainer::CreateLayer(Vec2::ANCHOR_BOTTOM_LEFT, , subSquareBaseContainer->getContentSize(), Color4B(SubSquareContainer::GetColorForNumer(value)), , containerIndex);*/
        subSquareContainer->GetTile()->setPosition(subSquareContainer->GetTile()->getBoundingBox().origin + subSquareContainer->GetTile()->getContentSize() / 2.0);
        subSquareContainer->GetTile()->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        subSquareContainer->GetTile()->setScale(1.0);
        subSquareContainer->SetSubSquareContainerValue(value);
        subSquareContainer->SetSubSquareContainerActive(true);

        log("New tile was created successfully at (%d, %d)  with value = %d", containerIndex.X, containerIndex.Y, value);

        // Number Text :
        Color3B textColor = value > 4 ? Game2048Common::_gameTheme->GetGameColors().TileText2 : Game2048Common::_gameTheme->GetGameColors().TileText1;
        Label * subSquareContainerText = CommonFunc::createLabel(CommonFunc::IntToString(value), Game2048::_gameLevels->GetSubSquareContainerDetails().FontFace, Game2048::_gameLevels->GetSubSquareContainerDetails().FontSize, 0, 0, Color3B::BLACK, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
        subSquareContainerText->setColor(textColor);
        subSquareContainerText->setName("SubSquareContainerText" + CommonFunc::IntToString(containerIndex.X) + "-" + CommonFunc::IntToString(containerIndex.Y));
        subSquareContainerText->setDimensions(subSquareContainer->GetTile()->getContentSize().width, subSquareContainer->GetTile()->getContentSize().height);
        subSquareContainer->GetTile()->addChild(subSquareContainerText, 5);

        // Touch Listener to the container:
        auto listenerSubSquareContainer = cocos2d::EventListenerTouchOneByOne::create();
        listenerSubSquareContainer->setSwallowTouches(false);
        listenerSubSquareContainer->onTouchBegan = [subSquareContainer](cocos2d::Touch* touch, cocos2d::Event* event) {
            if ((Game2048Common::_currentStage == Declarations::Stage::PlayGame) || (Game2048Common::_currentStage == Declarations::Stage::FTUE)) {
                if (CommonFunc::getWorldSpaceBoundingBox(subSquareContainer->GetTile()).containsPoint(touch->getLocation())) {
                    return true;
                }
            }
            return false;
        };
        listenerSubSquareContainer->onTouchEnded = [subSquareContainer](cocos2d::Touch* touch, cocos2d::Event* event) {
            Vec2 moveDelta = Vec2(abs(touch->getStartLocation().x - touch->getLocation().x), abs(touch->getStartLocation().x - touch->getLocation().x));
            float deltaXThreshold = 5.0;
            if (moveDelta.x <= deltaXThreshold && moveDelta.y <= deltaXThreshold)
            {
                if (!subSquareContainer->GetSubSquareContainerActive())
                {
                    GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                    subSquareContainer->GetTile()->stopActionByTag(1);
                    subSquareContainer->GetTile()->setScale(1.0);
                    Game2048::SetInfoMessage("Move the \"Atom\" by two tiles OR \nuse a diffuser to diffuse it.", subSquareContainer->GetTile());
                    // This number is disabled and it can't be added to another one! \nIt enables when it moves by two tiles.
                }
            }
        };
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerSubSquareContainer, subSquareContainer->GetTile());

        return subSquareContainer;
    }
}

void Game2048::SubSquareContainersMovement(Declarations::MovementDirection movementDirection)
{
    Game2048Common::_currentStage = Declarations::Stage::Unknown;

    int subSquareContainerPositionVeticalDelta = Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerSize.width + Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerMargin.x, subSquareContainerPositionHorizontalDelta = Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerSize.height + Game2048::_gameLevels->GetSubSquareContainerDetails().ContainerMargin.x;
    float subSquareContainerMoveAnimationTime = 0.100;// sec

    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    FiniteTimeAction * subSquareMovementAnimation = CallFunc::create([]() {});
    int incrementDirection = 0, numberOfSubSquareMovements = 0;
    if (movementDirection == Declarations::MovementDirection::Down || movementDirection == Declarations::MovementDirection::Up)
    { // Swiping in Y-Direction (Up / Down)
        //int totalSubSquareContainersMovements = 0;
        for (int xIndex = 0; xIndex < Game2048::_gameLevels->GetGameDimensions().X; xIndex++) {
            int subSquareContainerBlockDeltaYMove = 0;
            int prevSubSquareContainerValue = 0;
            bool prevSubSquareContainerActive = true;

            function<bool(int)> loopCondition = [movementDirection](int indexValue) {
                if (movementDirection == Declarations::MovementDirection::Down)
                {
                    return (indexValue <  Game2048::_gameLevels->GetGameDimensions().Y);
                }
                else if (movementDirection == Declarations::MovementDirection::Up) {
                    return (indexValue >= 0);
                }
                return false;
            };

            int yIndex = 0;
            if (movementDirection == Declarations::MovementDirection::Down)
            {
                incrementDirection = -1;
                yIndex = 0;
            }
            else if (movementDirection == Declarations::MovementDirection::Up) {
                incrementDirection = 1;
                yIndex = (Game2048::_gameLevels->GetGameDimensions().Y - 1);
            }
            for (; loopCondition(yIndex); yIndex -= incrementDirection) {
                SubSquareContainer * subSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));

                if (subSquareContainer && subSquareContainer->GetTile()) { // Text container found at that location.
                    int currentSubSquareContainerDeltaYMove = subSquareContainerBlockDeltaYMove;
                    FiniteTimeAction * currentSubSquareMovementAnimation = CallFunc::create([]() {});
                    FiniteTimeAction * targetCallBack = CallFunc::create([]() {});

                    Label * subSquareContainerText = subSquareContainer->GetTile()->getChildByName<Label*> ("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));

                    if (subSquareContainerText) {
                        if (subSquareContainer->GetSubSquareContainerActive() && prevSubSquareContainerActive && prevSubSquareContainerValue == CommonFunc::StringToInt(subSquareContainerText->getString())) {
                            // Both number matches while swiping, they will be merged now:
                            int prevSubSquareContainerValueHold = prevSubSquareContainerValue;
                            log("Matched Value : prevValue = %d, x = %d, y = %d", prevSubSquareContainerValue, xIndex, yIndex);
                            subSquareContainerBlockDeltaYMove += incrementDirection;
                            currentSubSquareContainerDeltaYMove += incrementDirection;
                            prevSubSquareContainerValue = 0;

                            targetCallBack = CallFunc::create([mainSquareContainer, xIndex, yIndex, subSquareContainer, subSquareContainerText, currentSubSquareContainerDeltaYMove, prevSubSquareContainerValueHold]() {
                                //log("CurrentIndex : prevValue = %d, x = %d, y = %d", prevSubSquareContainerValueHold, xIndex, yIndex);
                                int updatedValue = 2 * CommonFunc::StringToInt(subSquareContainerText->getString());
                                Game2048::_gameLevels->UpdatePlayerCurrentScore(updatedValue);
                                SdkboxPlayHelper::GetInstance()->SubmitScore(SdkboxPlayHelper::GetInstance()->GetLeaderboardNames().HighestScore, updatedValue);

                                // Higher tile created : Tiles merges here:
                                AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GamePlay, AppEvents::GetInstance()->GetEventAction().TilesMerged, "Tiles Merged", CommonFunc::StringToInt(subSquareContainerText->getString()));

                                int adjustedYIndex = yIndex + currentSubSquareContainerDeltaYMove;
                                SubSquareContainer * targetSubSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(adjustedYIndex));
                                //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(adjustedYIndex));
                                if (targetSubSquareContainer && targetSubSquareContainer->GetTile())
                                {
                                    targetSubSquareContainer->SetSubSquareContainerValue(updatedValue);
                                    if (Game2048::_isInactivatingOn && (Game2048::_inactiveSubContainerCounts < Game2048::_inactiveNumbers.size()) && (updatedValue == Game2048::_inactiveNumbers[Game2048::_inactiveSubContainerCounts]))
                                    {
                                        AdHelper::GetInstance()->ShowLateStartAndLightAds();
                                        // Tile will be disabled:
                                        targetSubSquareContainer->SetSubSquareContainerActive(false);
                                        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GamePlay, AppEvents::GetInstance()->GetEventAction().TileDisabled, "Tile Disabled", updatedValue);

                                        Game2048::_inactiveSubContainerCounts++;
                                        if (!Game2048::_inactiveInstructionShown)
                                        {
                                            Game2048::StartFTUE(Game2048::_disabledTileFTUETitle, targetSubSquareContainer->GetTile());
                                            // Node animations:
                                            FiniteTimeAction * highlightAnimation = RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.1), ScaleTo::create(0.5, 0.8), NULL));
                                            highlightAnimation->setTag(1);
                                            targetSubSquareContainer->GetTile()->runAction(highlightAnimation);
                                        }
                                    }
                                    else {
                                        targetSubSquareContainer->SetContainerColorForNum(updatedValue);
                                    }

                                    Label * targetSubSquareContainerText = targetSubSquareContainer->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(adjustedYIndex));
                                    if (targetSubSquareContainerText) {
                                        targetSubSquareContainerText->setString(CommonFunc::IntToString(updatedValue));
                                        if (updatedValue <= 4)
                                        {
                                            targetSubSquareContainerText->setColor(Game2048Common::_gameTheme->GetGameColors().TileText1);
                                        }
                                        else {
                                            targetSubSquareContainerText->setColor(Game2048Common::_gameTheme->GetGameColors().TileText2);
                                        }
                                    }
                                    // Visual Feedback when the tiles add up.
                                    //targetSubSquareContainer->runAction(Sequence::create(ScaleTo::create(0.100, 1.10), ScaleTo::create(0.100, 1.0), NULL));
                                }
                                //auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
                                //newGameContainer->runAction(RemoveSelf::create());
                                subSquareContainer->GetTile()->stopAllActions();
                                subSquareContainerText->stopAllActions();
                                subSquareContainer->RemoveSubSquareContainer();
                            });

                            if (abs(currentSubSquareContainerDeltaYMove) > 0) {
                                currentSubSquareMovementAnimation = TargetedAction::create(subSquareContainer->GetTile(), Sequence::create(MoveBy::create(subSquareContainerMoveAnimationTime, Vec2(0, currentSubSquareContainerDeltaYMove * subSquareContainerPositionVeticalDelta)), targetCallBack, /* DelayTime::create(0.210), */ NULL));
                            }
                        }
                        else {
                            if (subSquareContainer->GetSubSquareContainerActive()) {
                                prevSubSquareContainerActive = true;
                            }
                            else {
                                prevSubSquareContainerActive = false;
                            }
                            prevSubSquareContainerValue = CommonFunc::StringToInt(subSquareContainerText->getString());
                            targetCallBack = CallFunc::create([subSquareContainer, subSquareContainerText, xIndex, yIndex, currentSubSquareContainerDeltaYMove]() {
                                int adjustedyIndex = yIndex + currentSubSquareContainerDeltaYMove;
                                subSquareContainer->SetName("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(adjustedyIndex));
                                subSquareContainerText->setName("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(adjustedyIndex));

                                // Counting the moves of disabled Tile.
                                if (!subSquareContainer->GetSubSquareContainerActive()) // Inactive / Disabled:
                                {
                                    subSquareContainer->UpdateSubSquareContainerInactiveMovesCount(abs(currentSubSquareContainerDeltaYMove));
                                }

                                if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Normal)
                                {
                                }
                                else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::OneInactive)
                                {
                                    // If the SubSquareContainer was inactive and it is moved, then it becomes active.
                                    if ((abs(currentSubSquareContainerDeltaYMove) >= 2) && (!subSquareContainer->GetSubSquareContainerActive())) {
                                        // Enabling the tile (Activating is again);
                                        Game2048::CleanupInfoText();
                                        subSquareContainer->GetTile()->stopActionByTag(1);
                                        subSquareContainer->GetTile()->setScale(1.0);
                                        subSquareContainer->SetSubSquareContainerActive(true);
                                        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GamePlay, AppEvents::GetInstance()->GetEventAction().TileEnabled, "Tile Enabled", CommonFunc::StringToInt(subSquareContainerText->getString()));

                                        subSquareContainer->SetSubSquareContainerInactiveMovesCount(0);
                                        subSquareContainer->SetContainerColorForNum(CommonFunc::StringToInt(subSquareContainerText->getString()));
                                    }
                                }
                                else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Atom) {
                                    if ((!subSquareContainer->GetSubSquareContainerActive()) && (subSquareContainer->GetSubSquareContainerInactiveMovesCount() >= 2)) {
                                        // Set Atom active : False
                                        Game2048::SetAtomActive(subSquareContainer, false);
                                    }
                                }
                            });

                            if (abs(currentSubSquareContainerDeltaYMove) > 0) {
                                currentSubSquareMovementAnimation = TargetedAction::create(subSquareContainer->GetTile(), Sequence::create(MoveBy::create(subSquareContainerMoveAnimationTime, Vec2(0, currentSubSquareContainerDeltaYMove * subSquareContainerPositionVeticalDelta)), targetCallBack, NULL));
                            }
                        }
                    }

                    // Merging the current container to target container OR moving the current container by required places with respective animation:
                    if (abs(currentSubSquareContainerDeltaYMove) > 0) {
                        numberOfSubSquareMovements++;
                        subSquareMovementAnimation = Spawn::create(subSquareMovementAnimation, currentSubSquareMovementAnimation, NULL);
                    }
                }
                else { // subSquareContainer is empty
                    subSquareContainerBlockDeltaYMove += incrementDirection;
                }
            }
        }
    } else if (movementDirection == Declarations::MovementDirection::Left || movementDirection == Declarations::MovementDirection::Right) { // Swiping in X-direction (Left/Right)
        //int totalSubSquareContainersMovements = 0;
        for (int yIndex = 0; yIndex < Game2048::_gameLevels->GetGameDimensions().Y; yIndex++) {
            int subSquareContainerBlockDeltaXMove = 0;
            int prevSubSquareContainerValue = 0;
            bool prevSubSquareContainerActive = true;

            function<bool(int)> loopCondition = [movementDirection](int indexValue) {
                if (movementDirection == Declarations::MovementDirection::Left)
                {
                    return (indexValue < Game2048::_gameLevels->GetGameDimensions().X);
                }
                else if (movementDirection == Declarations::MovementDirection::Right) {
                    return (indexValue >= 0);
                }
                return false;
            };

            int xIndex = 0;
            if (movementDirection == Declarations::MovementDirection::Left)
            {
                incrementDirection = -1;
                xIndex = 0;
            }
            else if (movementDirection == Declarations::MovementDirection::Right) {
                incrementDirection = 1;
                xIndex = (Game2048::_gameLevels->GetGameDimensions().X - 1);
            }
            for (; loopCondition(xIndex); xIndex -= incrementDirection) {
                SubSquareContainer * subSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));

                if (subSquareContainer && subSquareContainer->GetTile()) { // Text container found at that location.
                    int currentSubSquareContainerDeltaXMove = subSquareContainerBlockDeltaXMove;
                    FiniteTimeAction * currentSubSquareMovementAnimation = CallFunc::create([]() {});
                    FiniteTimeAction * targetCallBack = CallFunc::create([]() {});

                    Label * subSquareContainerText = subSquareContainer->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));

                    if (subSquareContainerText) {
                        if (subSquareContainer->GetSubSquareContainerActive() && prevSubSquareContainerActive && prevSubSquareContainerValue == CommonFunc::StringToInt(subSquareContainerText->getString())) {
                            // SubSquareContainer (tile) should be active, previous tile should also be active,
                            // prev tile and current tile values should be same ==> Both the tiles will merge.
                            int prevSubSquareContainerValueHold = prevSubSquareContainerValue;
                            log("Matched Value : prevValue = %d, x = %d, y = %d", prevSubSquareContainerValue, xIndex, yIndex);
                            subSquareContainerBlockDeltaXMove += incrementDirection;
                            currentSubSquareContainerDeltaXMove += incrementDirection;
                            prevSubSquareContainerValue = 0;

                            targetCallBack = CallFunc::create([mainSquareContainer, xIndex, yIndex, subSquareContainer, subSquareContainerText, currentSubSquareContainerDeltaXMove, prevSubSquareContainerValueHold]() {
                                //log("CurrentIndex : prevValue = %d, x = %d, y = %d", prevSubSquareContainerValueHold, xIndex, yIndex);
                                int updatedValue = 2 * CommonFunc::StringToInt(subSquareContainerText->getString());
                                Game2048::_gameLevels->UpdatePlayerCurrentScore(updatedValue);
                                SdkboxPlayHelper::GetInstance()->SubmitScore(SdkboxPlayHelper::GetInstance()->GetLeaderboardNames().HighestScore, updatedValue);

                                int adjustedXIndex = xIndex + currentSubSquareContainerDeltaXMove;
                                SubSquareContainer * targetSubSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(adjustedXIndex) + "-" + CommonFunc::IntToString(yIndex));
                                //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(adjustedXIndex) + "-" + CommonFunc::IntToString(yIndex));
                                if (targetSubSquareContainer && targetSubSquareContainer->GetTile())
                                {
                                    targetSubSquareContainer->SetSubSquareContainerValue(updatedValue);
                                    if (Game2048::_isInactivatingOn && (Game2048::_inactiveSubContainerCounts < Game2048::_inactiveNumbers.size()) && (updatedValue == Game2048::_inactiveNumbers[Game2048::_inactiveSubContainerCounts]))
                                    {
                                        AdHelper::GetInstance()->ShowLateStartAndLightAds();
                                        // Tile will be disabled:
                                        targetSubSquareContainer->SetSubSquareContainerActive(false);
                                        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GamePlay, AppEvents::GetInstance()->GetEventAction().TileDisabled, "Tile Disabled", updatedValue);

                                        Game2048::_inactiveSubContainerCounts++;
                                        if (!Game2048::_inactiveInstructionShown)
                                        {
                                            Game2048::StartFTUE(Game2048::_disabledTileFTUETitle, targetSubSquareContainer->GetTile());
                                            // Node animations:
                                            FiniteTimeAction * highlightAnimation = RepeatForever::create(Sequence::create(ScaleTo::create(0.5, 1.1), ScaleTo::create(0.5, 0.8), NULL));
                                            highlightAnimation->setTag(1);
                                            targetSubSquareContainer->GetTile()->runAction(highlightAnimation);                                       
                                        }
                                    }
                                    else {
                                        targetSubSquareContainer->SetContainerColorForNum(updatedValue);
                                    }
                                    Label * targetSubSquareContainerText = targetSubSquareContainer->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(adjustedXIndex) + "-" + CommonFunc::IntToString(yIndex));
                                    if (targetSubSquareContainerText) {
                                        targetSubSquareContainerText->setString(CommonFunc::IntToString(updatedValue));
                                        if (updatedValue <= 4)
                                        {
                                            targetSubSquareContainerText->setColor(Game2048Common::_gameTheme->GetGameColors().TileText1);
                                        }
                                        else {
                                            targetSubSquareContainerText->setColor(Game2048Common::_gameTheme->GetGameColors().TileText2);
                                        }
                                    }
                                    //targetSubSquareContainer->runAction(Sequence::create(ScaleTo::create(0.100, 1.10), ScaleTo::create(0.100, 1.0), NULL));
                                }
                                subSquareContainer->GetTile()->stopAllActions();
                                subSquareContainerText->stopAllActions();
                                subSquareContainerText->removeFromParent();
                                subSquareContainer->GetTile()->removeFromParent();
                                subSquareContainer->RemoveSubSquareContainer();
                            });

                            if (abs(currentSubSquareContainerDeltaXMove) > 0) {
                                currentSubSquareMovementAnimation = TargetedAction::create(subSquareContainer->GetTile(), Sequence::create(MoveBy::create(subSquareContainerMoveAnimationTime, Vec2(currentSubSquareContainerDeltaXMove * subSquareContainerPositionHorizontalDelta, 0)), targetCallBack, /* DelayTime::create(0.210), */ NULL));
                            }
                        }
                        else {
                            // If either of the tiles (previous or current) are inactive or their values are not equal,
                            // then there would be no merger of tiles.
                            if (subSquareContainer->GetSubSquareContainerActive()) {
                                prevSubSquareContainerActive = true;
                            }
                            else {
                                prevSubSquareContainerActive = false;
                            }
                            prevSubSquareContainerValue = CommonFunc::StringToInt(subSquareContainerText->getString());
                            targetCallBack = CallFunc::create([subSquareContainer, subSquareContainerText, xIndex, yIndex, currentSubSquareContainerDeltaXMove]() {
                                int adjustedXIndex = xIndex + currentSubSquareContainerDeltaXMove;
                                subSquareContainer->SetName("SubSquareContainer" + CommonFunc::IntToString(adjustedXIndex) + "-" + CommonFunc::IntToString(yIndex));
                                subSquareContainerText->setName("SubSquareContainerText" + CommonFunc::IntToString(adjustedXIndex) + "-" + CommonFunc::IntToString(yIndex));

                                // Counting the moves of disabled Tile.
                                if (!subSquareContainer->GetSubSquareContainerActive())
                                {
                                    subSquareContainer->UpdateSubSquareContainerInactiveMovesCount(abs(currentSubSquareContainerDeltaXMove));
                                }

                                if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Normal)
                                {
                                }
                                else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::OneInactive)
                                {
                                    if ((abs(currentSubSquareContainerDeltaXMove) >= 2) && (!subSquareContainer->GetSubSquareContainerActive())) {
                                        Game2048::CleanupInfoText();
                                        subSquareContainer->GetTile()->stopActionByTag(1);
                                        subSquareContainer->GetTile()->setScale(1.0);
                                        subSquareContainer->SetSubSquareContainerActive(true);
                                        subSquareContainer->SetSubSquareContainerInactiveMovesCount(0);
                                        subSquareContainer->SetContainerColorForNum(CommonFunc::StringToInt(subSquareContainerText->getString()));
                                    }
                                }
                                else if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Atom) {
                                    if (!(subSquareContainer->GetSubSquareContainerActive()) && (subSquareContainer->GetSubSquareContainerInactiveMovesCount() >= 2))
                                    {
                                        // Set Atom active : False
                                        Game2048::SetAtomActive(subSquareContainer, false);
                                    }
                                }
                            });

                            if (abs(currentSubSquareContainerDeltaXMove) > 0) {
                                currentSubSquareMovementAnimation = TargetedAction::create(subSquareContainer->GetTile(), Sequence::create(MoveBy::create(subSquareContainerMoveAnimationTime, Vec2(currentSubSquareContainerDeltaXMove * subSquareContainerPositionHorizontalDelta, 0)), targetCallBack, NULL));
                            }
                        }
                    }

                    // Merging the current container to target container OR moving the current container by required places with respective animation:
                    if (abs(currentSubSquareContainerDeltaXMove) > 0) {
                        numberOfSubSquareMovements++;
                        subSquareMovementAnimation = Spawn::create(subSquareMovementAnimation, currentSubSquareMovementAnimation, NULL);
                    }
                }
                else { // subSquareContainer is empty
                    subSquareContainerBlockDeltaXMove += incrementDirection;
                }
            }
        }
    }

    // Playing the Movement animation and then choosing an element from the empty SubSquareContainers to fill it up.
    mainSquareContainer->runAction(Sequence::create(subSquareMovementAnimation, DelayTime::create(0.010), CallFunc::create([mainSquareContainer, numberOfSubSquareMovements]() {
        // Updating HUD for the previous movements:
        Game2048::UpdateHUD();

        string dataString = "";
        // Figuring out the empty subSquareContainers:
        vector<std::map<string, int>> vacantSubSquareContainers;
        int maxSubSquareContainerValue = 2;
        // Store the Empty containers separately.
        
        // Resetting atom tiles :
        Game2048::_gameLevels->DisableAtomTiles();

        for (int xIndex = 0; xIndex < Game2048::_gameLevels->GetGameDimensions().X; xIndex++)
        {
            for (int yIndex = 0; yIndex < Game2048::_gameLevels->GetGameDimensions().Y; yIndex++)
            {
                SubSquareContainer * subSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));
                if (subSquareContainer && subSquareContainer->GetTile()) {
                    Label * subSquareContainerText = subSquareContainer->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));
                    if (subSquareContainerText)
                    {
                        maxSubSquareContainerValue = MAX(maxSubSquareContainerValue, CommonFunc::StringToInt(subSquareContainerText->getString()));
                        dataString.append(CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex) + ":" + subSquareContainerText->getString() + "-" + CommonFunc::IntToString(subSquareContainer->GetSubSquareContainerActive()
                            ) + "-" + CommonFunc::IntToString(subSquareContainer->GetSubSquareContainerInactiveMovesCount()) + ";");

                        if (!subSquareContainer->GetSubSquareContainerActive()) {
                            Game2048::_gameLevels->SetAtomTile(subSquareContainer->GetSubSquareContainerActive(), xIndex, yIndex);
                        }
                    }
                }
                else {
                    std::map<string, int> vacantIndices;
                    vacantIndices[Declarations::_indexName.XIndex] = xIndex;
                    vacantIndices[Declarations::_indexName.YIndex] = yIndex;
                    vacantSubSquareContainers.push_back(vacantIndices);
                }
            }
        }

        if ((vacantSubSquareContainers.size() > 0) && (numberOfSubSquareMovements > 0)) { // There are empty SubSquareContainers and there was a movement with the previous scroll:
            int subSquareContainerToFillIndex = cocos2d::random(0, ((int)vacantSubSquareContainers.size() - 1));
            int subSquareContainerFillValue = CreateRandomValueForASubSquareContainer();
            log("Creating new tile at : (%d, %d) with value : %d", vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.XIndex], vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.YIndex], subSquareContainerFillValue);
            SubSquareContainer * subSquareContainer = Game2048::CreateSubSquareContainerWithValue(Game2048Common::Vec2I(vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.XIndex], vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.YIndex]), subSquareContainerFillValue);
            mainSquareContainer->addChild(subSquareContainer->GetTile(), 5);

            dataString.append(CommonFunc::IntToString(vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.XIndex]) + "-" + CommonFunc::IntToString(vacantSubSquareContainers[subSquareContainerToFillIndex][Declarations::_indexName.YIndex]) + ":" + CommonFunc::IntToString(subSquareContainerFillValue) + "-" + CommonFunc::IntToString(subSquareContainer->GetSubSquareContainerActive()) + "-" + CommonFunc::IntToString(subSquareContainer->GetSubSquareContainerInactiveMovesCount()) + ";");
        }
        _game2048HomeLayer->runAction(CallFunc::create([dataString]() {
            Game2048::_gameLevels->SaveSubSquareContainerValues(dataString);
        }));

        Game2048::CountPossibleMoves();

        // Game Winning Check:
        if (maxSubSquareContainerValue >= 2048 && !Game2048::_isGameWon) {
            Game2048::GameWonInitialization();
            return;
        }

        if ((Game2048::_subSquareContainerPossibleMovesCount.X == 0) && (Game2048::_subSquareContainerPossibleMovesCount.Y == 0)) { // There might be none empty subsquare containers OR there was no movement
               // In addition to that, there is no possible future movement.
            Game2048::GameOverInitialization();
            return;
        }

        Game2048Common::_currentStage = Declarations::Stage::PlayGame;

        // Starting automated testing: on screen cheat code - N-number of taps and then one swipe.
        // Test Section
        if (Game2048::_sceenTapCount >= 7)
        {
            Game2048::_isAutomatedTesting = true;
            Game2048::StartNewGame(GameLevels::GameLevel::Two, GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo::Normal);
        }
        else if (Game2048::_sceenTapCount >= 2)
        {
            // Stop Automated testing on 2 or more taps.
            if (Game2048::_isAutomatedTesting)
            {
                Game2048::StopAutomatedTesting();
            }
        }
        // Test Section Ends
        Game2048::_sceenTapCount = 0;
    }), NULL));
}

void Game2048::CountPossibleMoves() {
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    // One empty container has been filled if the above condition met, now Count how many moves are possible.
    Game2048::_subSquareContainerPossibleMovesCount = Game2048Common::ZEROI;
    for (int xIndex = 0; xIndex < Game2048::_gameLevels->GetGameDimensions().X; xIndex++)
    {
        for (int yIndex = 0; yIndex < Game2048::_gameLevels->GetGameDimensions().Y; yIndex++)
        {
            SubSquareContainer * subSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));
            if (subSquareContainer && subSquareContainer->GetTile()) {
                Label * subSquareContainerText = subSquareContainer->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));
                // Element check at "X+1";
                // Possible Moves in X-Direction.
                if ((xIndex + 1) < Game2048::_gameLevels->GetGameDimensions().X) {
                    int xPlusIndex = xIndex + 1;
                    SubSquareContainer * subSquareContainerXPlus = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xPlusIndex) + "-" + CommonFunc::IntToString(yIndex));
                    //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xPlusIndex) + "-" + CommonFunc::IntToString(yIndex));
                    if (subSquareContainerXPlus && subSquareContainerXPlus->GetTile())
                    {
                        if (subSquareContainer->GetSubSquareContainerActive() && subSquareContainerXPlus->GetSubSquareContainerActive())
                        {
                            Label * subSquareContainerTextXPlus = subSquareContainerXPlus->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xPlusIndex) + "-" + CommonFunc::IntToString(yIndex));
                            if (subSquareContainerText && subSquareContainerTextXPlus && (CommonFunc::StringToInt(subSquareContainerText->getString()) == CommonFunc::StringToInt(subSquareContainerTextXPlus->getString())))
                            {
                                Game2048::_subSquareContainerPossibleMovesCount.X++; // There may be some duoble counts here.
                            }
                        }
                    }
                    else {
                        Game2048::_subSquareContainerPossibleMovesCount.X++;
                    }
                }

                // Element Check at "X-1"
                // Possible Moves in X-Direction.
                if ((xIndex - 1) >= 0)
                {
                    int xMinusIndex = xIndex - 1;
                    SubSquareContainer * subSquareContainerXMinus = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xMinusIndex) + "-" + CommonFunc::IntToString(yIndex));
                    //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xMinusIndex) + "-" + CommonFunc::IntToString(yIndex));
                    if (subSquareContainerXMinus && subSquareContainerXMinus->GetTile())
                    {
                        if (subSquareContainer->GetSubSquareContainerActive() && subSquareContainerXMinus->GetSubSquareContainerActive())
                        {
                            Label * subSquareContainerTextXMinus = subSquareContainerXMinus->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xMinusIndex) + "-" + CommonFunc::IntToString(yIndex));
                            if (subSquareContainerText && subSquareContainerTextXMinus && (CommonFunc::StringToInt(subSquareContainerText->getString()) == CommonFunc::StringToInt(subSquareContainerTextXMinus->getString())))
                            {
                                Game2048::_subSquareContainerPossibleMovesCount.X++; // There may be some duoble counts here.
                            }
                        }
                    }
                    else {
                        Game2048::_subSquareContainerPossibleMovesCount.X++; // There may be some duoble counts here.
                    }
                }

                // Element Check at "Y+1"
                // Possible Moves in Y-Direction.
                if ((yIndex + 1) < Game2048::_gameLevels->GetGameDimensions().Y)
                {
                    int yPlusIndex = yIndex + 1;
                    SubSquareContainer * subSquareContainerYPlus = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yPlusIndex));
                    //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yPlusIndex));
                    if (subSquareContainerYPlus && subSquareContainerYPlus->GetTile())
                    {
                        if (subSquareContainer->GetSubSquareContainerActive() && subSquareContainerYPlus->GetSubSquareContainerActive())
                        {
                            Label * subSquareContainerTextYPlus = subSquareContainerYPlus->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yPlusIndex));
                            if (subSquareContainerText && subSquareContainerTextYPlus && (CommonFunc::StringToInt(subSquareContainerText->getString()) == CommonFunc::StringToInt(subSquareContainerTextYPlus->getString())))
                            {
                                Game2048::_subSquareContainerPossibleMovesCount.Y++; // There may be some duoble counts here.
                            }
                        }
                    }
                    else {
                        Game2048::_subSquareContainerPossibleMovesCount.Y++; // There may be some duoble counts here.
                    }
                }

                // Element Check at "Y-1"
                // Possible Moves in Y-Direction.
                if ((yIndex - 1) >= 0)
                {
                    int yMinusIndex = yIndex - 1;
                    SubSquareContainer * subSquareContainerYMinus = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yMinusIndex));
                    //SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yMinusIndex));
                    if (subSquareContainerYMinus && subSquareContainerYMinus->GetTile())
                    {
                        if (subSquareContainer->GetSubSquareContainerActive() && subSquareContainerYMinus->GetSubSquareContainerActive())
                        {
                            Label * subSquareContainerTextYMinus = subSquareContainerYMinus->GetTile()->getChildByName<Label*>("SubSquareContainerText" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yMinusIndex));
                            if (subSquareContainerText && subSquareContainerTextYMinus && (CommonFunc::StringToInt(subSquareContainerText->getString()) == CommonFunc::StringToInt(subSquareContainerTextYMinus->getString())))
                            {
                                Game2048::_subSquareContainerPossibleMovesCount.Y++; // There may be some duoble counts here.
                            }
                        }
                    }
                    else { // subSquare is empty:
                        Game2048::_subSquareContainerPossibleMovesCount.Y++; // There may be some duoble counts here.
                    }
                }
            }
            else {
                Game2048::_subSquareContainerPossibleMovesCount.Y++;
            }
        }
    }
}

void Game2048::CleanupAllTiles()
{
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    for (int xIndex = 0; xIndex < Game2048::_gameLevels->GetGameDimensions().X; xIndex++)
    {
        for (int yIndex = 0; yIndex < Game2048::_gameLevels->GetGameDimensions().Y; yIndex++)
        {
            SubSquareContainer * subSquareContainer = SubSquareContainer::GetSubSquareContainer("SubSquareContainer" + CommonFunc::IntToString(xIndex) + "-" + CommonFunc::IntToString(yIndex));
            if (subSquareContainer) {
                subSquareContainer->~SubSquareContainer();
            }
        }
    }
}

void Game2048::xtTapGesture(cocos2d::Point position) {
    if (Game2048Common::_currentStage == Declarations::Stage::PlayGame) {
        if (position.y < Game2048Common::_visibleSize.height/3.0)
        {
            Game2048::_sceenTapCount++;
        }
    }
}

void Game2048::xtSwipeGesture(XTTouchDirection direction, float distance, float speed) {
    if (!Game2048::_isAutomatedTesting && Game2048Common::_currentStage == Declarations::Stage::PlayGame)
    {
        if (direction == XTTouchDirection::UP)
        {
            Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Down);
        }
        else if (direction == XTTouchDirection::DOWN)
        {
            Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Up);
        }
        else if (direction == XTTouchDirection::RIGHT)
        {
            Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Left);
        }
        else if (direction == XTTouchDirection::LEFT)
        {
            Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Right);
        }
    }
}

// Back Key press handling (eg. on Android, windows devies)
void Game2048::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_BACK) {

        if (Game2048Common::_currentStage == Declarations::Stage::PlayGame) {
            // Stop Automated testing:
            if (Game2048::_isAutomatedTesting)
            {
                // The execution needs to retun, because there might be a Game Ove situation 
                // at this instant itself (in progress because of previous auto swipe), hence the back button needs to be pressed twice.
                Game2048::StopAutomatedTesting();
                return;
            }

            Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

            // Event:
            AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().HomeInteraction, AppEvents::GetInstance()->GetEventAction().ExitGame, "Game Exit initiated", 1);

            // With 25% transparency:
            Layer * gameExitConfirmationLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "GameExitConfirmationLayer");

            // Game Over text:
            Label * gameExitConfirmationText = CommonFunc::createLabel("Exit The Tile?", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, gameExitConfirmationLayer->getContentSize().width / 2.0, gameExitConfirmationLayer->getContentSize().height / 2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
            gameExitConfirmationText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
            gameExitConfirmationText->setDimensions(gameExitConfirmationLayer->getContentSize().width, gameExitConfirmationLayer->getContentSize().height / 4.0);

            // "Continue" button:
            Vec2 centerMargin = Vec2(20.0, 20.0);
            auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
            auto yesButton = CommonFunc::createSpriteFromCache("LightButton.png", gameExitConfirmationLayer->getContentSize().width / 2.0 - centerMargin.x, gameExitConfirmationLayer->getContentSize().height / 2.0 - centerMargin.y, "YesButton");
            yesButton->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);

            Label * yesButtonText = CommonFunc::createLabel("YES", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
            yesButtonText->setDimensions(yesButton->getContentSize().width, yesButton->getContentSize().height);
            yesButton->addChild(yesButtonText);

            auto listenerYesButtonText = cocos2d::EventListenerTouchOneByOne::create();
            listenerYesButtonText->setSwallowTouches(true);
            listenerYesButtonText->onTouchBegan = [yesButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
                if (Game2048Common::_currentStage == Declarations::Stage::GameExitConfirmation) {
                    if (CommonFunc::getWorldSpaceBoundingBox(yesButtonText).containsPoint(touch->getLocation())) {
                        // Register Event:
                        Game2048Common::_currentStage = Declarations::Stage::Unknown;
                        //                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((CommonFunc::_soundPath + "ButtonClick.mp3").c_str());
                        //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonPressed.png");
                        return true;
                    }
                }
                return false;
            };

            listenerYesButtonText->onTouchEnded = [gameExitConfirmationLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
                //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonUnpressed.png");
                // Event:
                AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().ExitGame, AppEvents::GetInstance()->GetEventAction().ExitedGame, "Exited from Game", 1);

                Director::getInstance()->end();
            };
            cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerYesButtonText, yesButtonText);

            // No Button.
            auto noButton = CommonFunc::createSpriteFromCache("HeavyButton.png", gameExitConfirmationLayer->getContentSize().width / 2.0 + centerMargin.x, gameExitConfirmationLayer->getContentSize().height / 2.0 - centerMargin.y, "noButton");
            noButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

            Label * noButtonText = CommonFunc::createLabel("NO", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
            noButtonText->setDimensions(noButton->getContentSize().width, noButton->getContentSize().height);
            noButton->addChild(noButtonText);

            auto listenerNoButtonText = cocos2d::EventListenerTouchOneByOne::create();
            listenerNoButtonText->setSwallowTouches(true);
            listenerNoButtonText->onTouchBegan = [noButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
                if (Game2048Common::_currentStage == Declarations::Stage::GameExitConfirmation) {
                    if (CommonFunc::getWorldSpaceBoundingBox(noButtonText).containsPoint(touch->getLocation())) {
                        // Register Event:
                        Game2048Common::_currentStage = Declarations::Stage::Unknown;
                        //                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((CommonFunc::_soundPath + "ButtonClick.mp3").c_str());
                        //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonPressed.png");
                        return true;
                    }
                }
                return false;
            };

            listenerNoButtonText->onTouchEnded = [gameExitConfirmationLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
                //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonUnpressed.png");
                // Event:
                AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().ExitGame, AppEvents::GetInstance()->GetEventAction().StaysInGame, "Stays in Game", 1);

                gameExitConfirmationLayer->runAction(Sequence::create(CallFunc::create([]() {
                    Game2048Common::_currentStage = Declarations::Stage::PlayGame;
                }), RemoveSelf::create(), NULL));
            };
            cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerNoButtonText, noButtonText);

            gameExitConfirmationLayer->addChild(noButton, 10);
            gameExitConfirmationLayer->addChild(yesButton, 10);
            gameExitConfirmationLayer->addChild(gameExitConfirmationText, 10);
            _game2048HomeLayer->addChild(gameExitConfirmationLayer, 100);
            Game2048Common::_currentStage = Declarations::Stage::GameExitConfirmation;
        }
        else if (Game2048Common::_currentStage == Declarations::Stage::GameExitConfirmation) {
            auto gameExitConfirmationLayer = _game2048HomeLayer->getChildByName<Layer*>("GameExitConfirmationLayer");
            gameExitConfirmationLayer->runAction(RemoveSelf::create());
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }
        else if (Game2048Common::_currentStage == Declarations::Stage::InitializeDiffusersScreen) {
            auto initializeDiffusersScreenLayer = _game2048HomeLayer->getChildByName<Layer*>("InitializeDiffusersScreenLayer");
            initializeDiffusersScreenLayer->runAction(RemoveSelf::create());
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }
        else if (Game2048Common::_currentStage == Declarations::Stage::InitializeGameTypeChange) {
            auto initializeGameTypeChangeLayer = _game2048HomeLayer->getChildByName<Layer*>("InitializeGameTypeChangeLayer");
            initializeGameTypeChangeLayer->runAction(RemoveSelf::create());
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }
    }
}

void Game2048::UpdateHUD() {
    // Curren Score Update:
    Sprite * currentScoreContainer = _homeScreenLayer->getChildByName<Sprite*>("CurrentScoreContainer");
    Label * currentScoreContainerText = currentScoreContainer->getChildByName<Label*>("CurrentScoreContainerTextScore");
    if (currentScoreContainerText) {
        if (currentScoreContainerText->getString().compare(CommonFunc::IntToString(Game2048::_gameLevels->GetPlayerCurrentScore())) != 0)
        {
            currentScoreContainerText->setString(CommonFunc::IntToString(Game2048::_gameLevels->GetPlayerCurrentScore()));
            currentScoreContainerText->runAction(Sequence::create(ScaleTo::create(0.25, 1.5), ScaleTo::create(0.25, 1.0),NULL));
        }
    }

    // Best Score:
    Sprite * bestScoreContainer = _homeScreenLayer->getChildByName<Sprite*>("BestScoreContainer");
    Label * bestScoreContainerText = bestScoreContainer->getChildByName<Label*>("BestScoreContainerTextScore");
    if (bestScoreContainerText) {
        bestScoreContainerText->setString(CommonFunc::IntToString(Game2048::_gameLevels->GetPlayerBestScore()));
    }

    // Atom diffuser count :
    auto diffuserContainer = _homeScreenLayer->getChildByName<Sprite*>("DiffuserContainer");
    Label * diffuserContainerText = diffuserContainer->getChildByName<Label*>("DiffuserContainerText");
    diffuserContainerText->setString(CommonFunc::IntToString(Player::GetInstance()->GetDiffusersCount()));
}

void Game2048::GameOverInitialization() {
    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameOver, AppEvents::GetInstance()->GetEventAction().GameOver, "Game is Over", 1);

    Game2048::StopTimer();
    Game2048::_gameLevels->DeleteSubSquareContainerValues();
    Game2048::_gameLevels->SetPlayerCurrentScore(0);

    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    
    Layer * gameOverLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "GameOverLayer");

    // Game Over text:
    Label * gameOverText = CommonFunc::createLabel("Game Over!", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, gameOverLayer->getContentSize().width / 2.0, gameOverLayer->getContentSize().height / 2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    gameOverText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    gameOverText->setDimensions(gameOverLayer->getContentSize().width, gameOverLayer->getContentSize().height / 4.0);

    // "Try again" button:
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    auto tryAgainButtonHolder = CommonFunc::createSpriteFromCache("AtomModeButton.png", gameOverLayer->getContentSize().width / 2.0, (5.0 / 6.0) * gameOverLayer->getContentSize().height / 2.0, "TryAgainButtonHolder");
    tryAgainButtonHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    auto listenerTryAgainButtonHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerTryAgainButtonHolder->setSwallowTouches(true);
    listenerTryAgainButtonHolder->onTouchBegan = [tryAgainButtonHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::GameOver) {
            if (CommonFunc::getWorldSpaceBoundingBox(tryAgainButtonHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(tryAgainButtonHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerTryAgainButtonHolder->onTouchEnded = [gameOverLayer, tryAgainButtonHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(tryAgainButtonHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameOver, AppEvents::GetInstance()->GetEventAction().TryAgain, "Another game try initiated", 1);

        Game2048::StartNewGame(Game2048::_gameLevels->GetGameLevel(), Game2048::_gameLevels->GetDifficultyLevel(), Game2048::_gameLevels->GetGameAlgo());

        // Asking users to provide feedback and rate the app of app store.
        // Since the game is tough, users are not able to score 2048, hence not winning and could not be 
        // diverted to rate the app thought winning scenario.
        // Hence we are asking users to rate our app when they have played more than 5/10 sessions.

        int sessionCount = UserDefault::getInstance()->getIntegerForKey(KeyNames::GetInstance()->GetKeyName().SessionCount.c_str(), 0);

        int rateNotNowDayNum = UserDefault::getInstance()->getIntegerForKey("RateNotNowDayNum", 0);
        bool isRatedTheGame = UserDefault::getInstance()->getBoolForKey("IsRatedTheGame", false);

        // When game is over, and if user was not asked to rate the game today (in last 24hours), then ask user to 
        // rate the game on app store.
        // Ask only after sessions : 3 ;
        if (!isRatedTheGame && (Game2048Common::GetNumberOfDaysSinceBeginning() >= (rateNotNowDayNum + 1)) && (sessionCount >= 3))
        {
            Game2048::InitializeRateTheApp();
        }
        else
        {
            // Showing ads when user loses the game and starts another try.
            // 1 out of three ads will be shown as a video ad.
            AdHelper::GetInstance()->ShowInterstitialWeightedAd();
        }

        gameOverLayer->runAction(RemoveSelf::create());
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerTryAgainButtonHolder, tryAgainButtonHolder);

    Label * tryAgainButtonText = CommonFunc::createLabel("Try Again", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    tryAgainButtonText->setDimensions(tryAgainButtonHolder->getContentSize().width, tryAgainButtonHolder->getContentSize().height);
    tryAgainButtonHolder->addChild(tryAgainButtonText, 10);

    gameOverLayer->addChild(tryAgainButtonHolder, 10);
    gameOverLayer->addChild(gameOverText, 10);
    _game2048HomeLayer->addChild(gameOverLayer, 100);
    Game2048Common::_currentStage = Declarations::Stage::GameOver;
}

void Game2048::GameWonInitialization()
{
    if (Game2048::_isAutomatedTesting)
    {
        return;
    }

    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameWon, AppEvents::GetInstance()->GetEventAction().GameWon, "Game is Won", 1);

    Game2048::_isGameWon = true;
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    // With 25% transparency:
    Layer * gameWonLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "GameWonLayer");

    // Game Over text:
    Label * gameWonText = CommonFunc::createLabel("You Win!", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, gameWonLayer->getContentSize().width / 2.0, gameWonLayer->getContentSize().height / 2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    gameWonText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    gameWonText->setDimensions(gameWonLayer->getContentSize().width, gameWonLayer->getContentSize().height / 4.0);

    // "Continue" button:
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    auto continueButtonHolder = CommonFunc::createSpriteFromCache("AtomModeButton.png", gameWonLayer->getContentSize().width / 2.0, gameWonLayer->getContentSize().height / 2.0 - 20.0, "ContinueButtonHolder");
    continueButtonHolder->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    auto listenerContinueButtonHolder = cocos2d::EventListenerTouchOneByOne::create();
    listenerContinueButtonHolder->setSwallowTouches(true);
    listenerContinueButtonHolder->onTouchBegan = [continueButtonHolder](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::GameWon) {
            if (CommonFunc::getWorldSpaceBoundingBox(continueButtonHolder).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(ContinueButtonHolder, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerContinueButtonHolder->onTouchEnded = [gameWonLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(ContinueButtonHolder, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameWon, AppEvents::GetInstance()->GetEventAction().GameContinues, "Game Continues", 1);

        gameWonLayer->runAction(Sequence::create(CallFunc::create([]() {
            int rateNotNowDayNum = UserDefault::getInstance()->getIntegerForKey("RateNotNowDayNum", 0);
            bool isRatedTheGame = UserDefault::getInstance()->getBoolForKey("IsRatedTheGame", false);

            // When game is won, and if user was not asked rate the game today (in last 24hours), then ask user to 
            // rate the game on app store.
            if (!isRatedTheGame && (Game2048Common::GetNumberOfDaysSinceBeginning() >= (rateNotNowDayNum + 1)))
            {
                Game2048::InitializeRateTheApp();
            }
            else {
                // Showing a video ad when user presses "Continue" on game won page.
                AdHelper::GetInstance()->ShowAd(AdHelper::GetInstance()->GetAdNames().InterstitialVideoOnly);

                Game2048Common::_currentStage = Declarations::Stage::PlayGame;
            }
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerContinueButtonHolder, continueButtonHolder);

    Label * continueButtonText = CommonFunc::createLabel("CONTINUE", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    continueButtonText->setDimensions(continueButtonHolder->getContentSize().width, continueButtonHolder->getContentSize().height);
    continueButtonHolder->addChild(continueButtonText, 10);

    gameWonLayer->addChild(continueButtonHolder, 10);
    gameWonLayer->addChild(gameWonText, 10);
    _game2048HomeLayer->addChild(gameWonLayer, 100);
    Game2048Common::_currentStage = Declarations::Stage::GameWon;
}

void Game2048::InitializeRateTheApp()
{
/*     Function Description:
      When selected "YES"->set "IsRatedTheGame" flag = true.
      Redirect to PlayStore.
      Set game Stage to PlayGame to continue playing.
      --> thankYou page will not be added for now.

      if "NOT NOW" selected -> set that day as "RateNotNowDayNum".
      Set game Stage to PlayGame to continue playing. */

      // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().RateTheApp, AppEvents::GetInstance()->GetEventAction().RateTheApp, "Rate the app initiated", 1);

    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    // With 25% transparency:
    Layer * rateTheAppLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "RateTheAppLayer");

    // Game Over text:
    Label * rateTheAppText = CommonFunc::createLabel("Loved it!\nRate The Tile?", Game2048Common::_gameTheme->GetFontFace().Bold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, rateTheAppLayer->getContentSize().width / 2.0, rateTheAppLayer->getContentSize().height / 2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    rateTheAppText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    rateTheAppText->setDimensions(rateTheAppLayer->getContentSize().width, rateTheAppLayer->getContentSize().height / 4.0);

    // "Continue" button:
    Vec2 centerMargin = Vec2(20.0, 20.0);
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    auto yesButtonHolder = CommonFunc::createSpriteFromCache("HeavyButton.png", rateTheAppLayer->getContentSize().width / 2.0 + centerMargin.x, rateTheAppLayer->getContentSize().height / 2.0 - centerMargin.y, "YesButtonHolder");
    yesButtonHolder->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

    Label * yesButtonText = CommonFunc::createLabel("YES", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    yesButtonText->setDimensions(yesButtonHolder->getContentSize().width, yesButtonHolder->getContentSize().height);
    yesButtonHolder->addChild(yesButtonText);

    auto listenerYesButtonText = cocos2d::EventListenerTouchOneByOne::create();
    listenerYesButtonText->setSwallowTouches(true);
    listenerYesButtonText->onTouchBegan = [yesButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::RateTheApp) {
            if (CommonFunc::getWorldSpaceBoundingBox(yesButtonText).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerYesButtonText->onTouchEnded = [rateTheAppLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().RateTheApp, AppEvents::GetInstance()->GetEventAction().AgreesToRate, "Agrees to rate the app", 1);

        rateTheAppLayer->runAction(Sequence::create(CallFunc::create([]() {
            UserDefault::getInstance()->setBoolForKey("IsRatedTheGame", true);
            CommonFunc::RedirectToAppStoreDetailsTab(Declarations::_AppID);
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerYesButtonText, yesButtonText);

    auto noButtonHolder = CommonFunc::createSpriteFromCache("LightButton.png", rateTheAppLayer->getContentSize().width / 2.0 - centerMargin.x, rateTheAppLayer->getContentSize().height / 2.0 - centerMargin.y, "NoButtonHolder");
    noButtonHolder->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    noButtonHolder->setOpacity(0);

    Label * noButtonText = CommonFunc::createLabel("NOT NOW", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Medium, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    noButtonText->setDimensions(noButtonHolder->getContentSize().width, noButtonHolder->getContentSize().height);
    noButtonHolder->addChild(noButtonText);

    auto listenerNoButtonText = cocos2d::EventListenerTouchOneByOne::create();
    listenerNoButtonText->setSwallowTouches(true);
    listenerNoButtonText->onTouchBegan = [noButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::RateTheApp) {
            if (CommonFunc::getWorldSpaceBoundingBox(noButtonText).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerNoButtonText->onTouchEnded = [rateTheAppLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().RateTheApp, AppEvents::GetInstance()->GetEventAction().NotAgreeToRate, "Not agree to rate the app", 1);

        rateTheAppLayer->runAction(Sequence::create(CallFunc::create([]() {
            UserDefault::getInstance()->setIntegerForKey("RateNotNowDayNum", Game2048Common::GetNumberOfDaysSinceBeginning());
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerNoButtonText, noButtonText);

    rateTheAppLayer->addChild(noButtonHolder, 10);
    rateTheAppLayer->addChild(yesButtonHolder, 10);
    rateTheAppLayer->addChild(rateTheAppText, 10);
    _game2048HomeLayer->addChild(rateTheAppLayer, 100);
    Game2048Common::_currentStage = Declarations::Stage::RateTheApp;
}

void Game2048::NewGameConfirmationPopup()
{
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");

    // With 25% transparency:
    Layer * newGameConfirmationLayer = CommonFunc::CreateLayerColor(Vec2::ANCHOR_BOTTOM_LEFT, CommonFunc::getWorldSpaceBoundingBox(mainSquareContainer).origin, mainSquareContainer->getContentSize(), Game2048Common::_gameTheme->GetGameColors().PopupColor, "NewGameConfirmationLayer");

    // Game Over text:
    Label * newGameConfirmationText = CommonFunc::createLabel("Restart?", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().ExtraLarge, newGameConfirmationLayer->getContentSize().width / 2.0, newGameConfirmationLayer->getContentSize().height / 2.0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    newGameConfirmationText->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    newGameConfirmationText->setDimensions(newGameConfirmationLayer->getContentSize().width, newGameConfirmationLayer->getContentSize().height / 4.0);

    // "Continue" button:
    Vec2 centerMargin = Vec2(20.0, 20.0);
    auto newGameContainer = _homeScreenLayer->getChildByName<Sprite*>("NewGameContainer");
    auto yesButtonHolder = CommonFunc::createSpriteFromCache("LightButton.png", newGameConfirmationLayer->getContentSize().width / 2.0 - centerMargin.x, newGameConfirmationLayer->getContentSize().height / 2.0 - centerMargin.y, "YesButtonHolder");
    yesButtonHolder->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);

    Label * yesButtonText = CommonFunc::createLabel("YES", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Game2048Common::_gameTheme->GetGameColors().ThemeTextWhite, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    yesButtonText->setDimensions(yesButtonHolder->getContentSize().width, yesButtonHolder->getContentSize().height);
    yesButtonHolder->addChild(yesButtonText);

    auto listenerYesButtonText = cocos2d::EventListenerTouchOneByOne::create();
    listenerYesButtonText->setSwallowTouches(true);
    listenerYesButtonText->onTouchBegan = [yesButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::NewGameConfirmation) {
            if (CommonFunc::getWorldSpaceBoundingBox(yesButtonText).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerYesButtonText->onTouchEnded = [newGameConfirmationLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(yesButtonText, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().RestartGame, AppEvents::GetInstance()->GetEventAction().RestartedGame, "Game Restarted", 1);

        // Showing a video ad when user presses "Yes" on game re-start page.
        // 1 out of three ads will be shown as a video ad.
        AdHelper::GetInstance()->ShowInterstitialWeightedAd();

        newGameConfirmationLayer->runAction(Sequence::create(CallFunc::create([]() {
            Game2048::StartNewGame(GameLevels::GameLevel::Four, Game2048::_gameLevels->GetDifficultyLevel(), Game2048::_gameLevels->GetGameAlgo());
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerYesButtonText, yesButtonText);

    auto noButtonHolder = CommonFunc::createSpriteFromCache("HeavyButton.png", newGameConfirmationLayer->getContentSize().width / 2.0 + centerMargin.x, newGameConfirmationLayer->getContentSize().height / 2.0 - centerMargin.y, "NoButtonHolder");
    noButtonHolder->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

    Label * noButtonText = CommonFunc::createLabel("NO", Game2048Common::_gameTheme->GetFontFace().ExtraBold, Game2048Common::_gameTheme->GetFontSize().Large, 0, 0, Color3B::WHITE, cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    noButtonText->setDimensions(noButtonHolder->getContentSize().width, noButtonHolder->getContentSize().height);
    noButtonHolder->addChild(noButtonText);

    auto listenerNoButtonText = cocos2d::EventListenerTouchOneByOne::create();
    listenerNoButtonText->setSwallowTouches(true);
    listenerNoButtonText->onTouchBegan = [noButtonText](cocos2d::Touch* touch, cocos2d::Event* event) {
        if (Game2048Common::_currentStage == Declarations::Stage::NewGameConfirmation) {
            if (CommonFunc::getWorldSpaceBoundingBox(noButtonText).containsPoint(touch->getLocation())) {
                // Register Event:
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().ButtonClick);
                //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonPressed.png");
                return true;
            }
        }
        return false;
    };

    listenerNoButtonText->onTouchEnded = [newGameConfirmationLayer](cocos2d::Touch* touch, cocos2d::Event* event) {
        //CommonFunc::changeSpriteImage(noButtonText, "ClosePopUpBoxButtonUnpressed.png");
        // Event:
        AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().RestartGame, AppEvents::GetInstance()->GetEventAction().NotRestartedGame, "Game Not Restarted", 1);

        newGameConfirmationLayer->runAction(Sequence::create(CallFunc::create([]() {
            Game2048Common::_currentStage = Declarations::Stage::PlayGame;
        }), RemoveSelf::create(), NULL));
    };
    cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerNoButtonText, noButtonText);

    newGameConfirmationLayer->addChild(noButtonHolder, 10);
    newGameConfirmationLayer->addChild(yesButtonHolder, 10);
    newGameConfirmationLayer->addChild(newGameConfirmationText, 10);
    _game2048HomeLayer->addChild(newGameConfirmationLayer, 100);
    Game2048Common::_currentStage = Declarations::Stage::NewGameConfirmation;
}

void Game2048::StartNewGame(int gameLevel, GameLevels::GameDifficultyLevel gameDifficultyLevel, GameLevels::GameAlgo gameAlgo)
{
    auto gameOverLayer = Game2048::_game2048HomeLayer->getChildByName<Layer*>("GameOverLayer");
    if (gameOverLayer)
    {
        gameOverLayer->runAction(RemoveSelf::create());
    }
    auto gameWonLayer = Game2048::_game2048HomeLayer->getChildByName<Layer*>("GameWonLayer");
    if (gameWonLayer)
    {
        gameWonLayer->runAction(RemoveSelf::create());
    }

    Game2048::_gameLevels->SetPlayerCurrentScore(0);
    Game2048::ResetGameSetting();

    Game2048::SetGameSettings(gameLevel, gameDifficultyLevel, gameAlgo);
    Game2048::InitializeHomeScreen();
}

void Game2048::StartAutomatedTesting() {
    Sprite * mainSquareContainer = _homeScreenLayer->getChildByName<Sprite*>("MainSquareContainer");
    float autoPlayDeltaT = 0.500;

    FiniteTimeAction * autoTestingSequence = RepeatForever::create(Sequence::create(DelayTime::create(autoPlayDeltaT), CallFunc::create([mainSquareContainer]() {
        if (Game2048::_isAutomatedTesting)
        {
            if (Game2048Common::_currentStage != Declarations::Stage::GameOver) {
                if ((Game2048::_subSquareContainerPossibleMovesCount.X != 0) && (Game2048::_subSquareContainerPossibleMovesCount.Y != 0)) { // Swipe is possible in either direction.
                    int numToSpawn = random(0, 100);

                    if (numToSpawn <= 25) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Down);
                    }
                    else if (numToSpawn <= 50) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Left);
                    }
                    else if (numToSpawn <= 75) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Up);
                    }
                    else if (numToSpawn <= 100) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Right);
                    }
                }
                else if (Game2048::_subSquareContainerPossibleMovesCount.X != 0) { // Swipe only possible in X-direction.
                    int numToSpawn = random(0, 100);

                    if (numToSpawn <= 50) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Left);
                    }
                    else if (numToSpawn <= 100) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Right);
                    }
                }
                else if (Game2048::_subSquareContainerPossibleMovesCount.Y != 0) { // Swipe only possible in Y-direction.
                    int numToSpawn = random(0, 100);

                    if (numToSpawn <= 50) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Down);
                    }
                    else if (numToSpawn <= 100) {
                        Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Up);
                    }
                }
                else {
                    // Making one swipe, which will make the game over since there are no movements possible.
                    Game2048::SubSquareContainersMovement(Declarations::MovementDirection::Down);
                }
            }
            else {
                Game2048Common::_currentStage = Declarations::Stage::Unknown;
                mainSquareContainer->stopActionByTag(Declarations::ActionTag::AutoTesting);

                mainSquareContainer->runAction(Sequence::create(DelayTime::create(0.5), CallFunc::create([]() {
                    if (Game2048::_gameLevels->GetGameAlgo() == GameLevels::GameAlgo::Normal)
                    {
                        // Test the Atom mode of the same level
                        Game2048::_gameLevels->SetGameAlgo(GameLevels::GameAlgo::Atom);
                    }
                    else {
                        // When the atom mode is tested, then test the next level.
                        Game2048::_gameLevels->SetGameAlgo(GameLevels::GameAlgo::Normal);
                        Game2048::_gameLevels->SetGameLevel(GameLevels::GameLevel(Game2048::_gameLevels->GetGameLevel() + 1));
                    }
                    if (Game2048::_gameLevels->GetGameLevel() <= GameLevels::GameLevel::Ten)
                    {
                        Game2048::StartNewGame(Game2048::_gameLevels->GetGameLevel(), Game2048::_gameLevels->GetDifficultyLevel(), Game2048::_gameLevels->GetGameAlgo());
                    }
                }), NULL));
            }
        }
        else {
            mainSquareContainer->stopActionByTag(Declarations::ActionTag::AutoTesting);
        }
    }), NULL));
    autoTestingSequence->setTag(Declarations::ActionTag::AutoTesting);
    mainSquareContainer->runAction(autoTestingSequence);
}

void Game2048::StopAutomatedTesting()
{
    Game2048::_isAutomatedTesting = false;
}
