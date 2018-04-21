// Created by Radhey Shyam Meena
// March 2017.

#ifndef __GAME2048_SCENE_H__
#define __GAME2048_SCENE_H__

#include "cocos2d.h"
#include "Declarations.h"
#include "Game2048Common.h"
#include "XTLayer.h"
#include "GameLevels.h"
#include "SubSquareContainer.h"

class Game2048 : public XTLayer
{
public:

    static Declarations _declarations;
    static Layer * _game2048HomeLayer;
    static Layer * _homeScreenLayer;
    static GameLevels * _gameLevels;
    static int _sceenTapCount;
    static unsigned int _inactiveSubContainerCounts;
    static vector<int> _inactiveNumbers;
    static bool _isAutomatedTesting, _isGameWon, _isInactivatingOn, _inactiveInstructionShown, _isModeChangeActive;
    static Game2048Common::Vec2I _subSquareContainerPossibleMovesCount;
    static float _allTilesActiveTime;

    // String:
    static string _disabledTileFTUETitle, _timerBoxFTUETitle;


    static cocos2d::Scene* createScene();

    virtual bool init();

    static void SetGameInfo();

    static void SetGameSettings(int gameLevel, GameLevels::GameDifficultyLevel difficultyLevel, GameLevels::GameAlgo gameAlgo);

    static void ResetGameSetting();

    static void SetInactiveNumbers();
    static void ResetInactiveNumbers();

    static void InitializeHomeScreen();

    static void InitializeGameTypeChange();

    static void InitializeDiffusersScreen();

    static void SetAtomActive(SubSquareContainer * subSquareContainer, bool isActive);

    static void CleanupInfoText();
    // start First Time User Experience (FTUE):
    static void StartFTUE(string titleText, Node * nodeFTUE);
    static void SetInfoMessage(string infoTextMessage, Node * node);
    static string GetNormalModeActiveMessage(float time);

    static void StartTimer(float totalTime, function<void()> callback);
    static void StopTimer();

    static int CreateRandomValueForASubSquareContainer();
    static SubSquareContainer * CreateSubSquareContainerWithValue(Game2048Common::Vec2I containerIndex, int value);

    static void SubSquareContainersMovement(Declarations::MovementDirection movementDirection);

    static void CountPossibleMoves();

    static void CleanupAllTiles();

    // Update Score and HUD section:
    static void UpdateHUD();

    static void GameOverInitialization();

    static void GameWonInitialization();

    static void InitializeRateTheApp();

    static void NewGameConfirmationPopup();

    static void StartNewGame(int gameLevel = GameLevels::GameLevel::Four, GameLevels::GameDifficultyLevel gameDifficultyLevel = GameLevels::GameDifficultyLevel::Easy, GameLevels::GameAlgo gameAlgo = GameLevels::GameAlgo::Atom);

    virtual void xtTapGesture(cocos2d::Point position);
    virtual void xtSwipeGesture(XTTouchDirection direction, float distance, float speed);

    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) override;

    static void StartAutomatedTesting();
    static void StopAutomatedTesting();

    // implement the "static create()" method manually
    CREATE_FUNC(Game2048);

protected:

};

#endif // __GAME2048_SCENE_H__
