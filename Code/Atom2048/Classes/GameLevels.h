// Created by Radhey Shyam Meena
// March 2017.

#ifndef GameLevels_hpp
#define GameLevels_hpp

#include "Game2048Common.h"
#include "Common.h"
#include "SubSquareContainer.h"

using namespace std;

class GameLevels
{
public:
    GameLevels();
    virtual ~GameLevels();

    struct GameDimensions
    {
        int X;
        int Y;
    };

    enum GameDifficultyLevel {
        Easy = 1,
        Medium = 2,
        Hard = 3
    };

    enum GameLevel {
        Two = 2,
        Three = 3,
        Four = 4,
        Five = 5,
        Six = 6,
        Seven = 7,
        Eight = 8,
        Nine = 9,
        Ten = 10
    };

    struct DifficultyLevel
    {
        string Easy = "Easy";
        string Medium = "Medium";
        string Hard = "Hard";
    };

    struct PlayerData {
        int CurrentScore = 0;
        int BestScore = 0;
    };

    struct SubSquareContainerDetails {
        float FontSize;
        string FontFace;
        Size ContainerSize;
        Vec2 ContainerMargin;
    };

    // Game Properties:
    enum GameAlgo {
        Normal,
        // One num (large number. eg 512) becomes inactive and 
        // when it is moved by two places at once, it would become active.
        OneInactive,

        // Maximum Two numbers (small numbers. eg. 32) can be inactive at a time.
        // They become active when moved by two places[at once or in multiple moves].
        Atom

    };

    struct GameMode {
        string Normal = "NORMAL";
        string Atom = "ATOM";
    };

    struct GameType {
        // GameLevel, GameDifficulty, GameType, GameDifficultyLevelName, GameDimensions.
        int GameLevel = GameLevels::GameLevel::Four;
        GameDifficultyLevel DifficultyLevel;
        GameAlgo Algo = GameLevels::GameAlgo::Atom;
        string Mode; // GameLevels::GameMode
        string DifficultyLevelName;
        GameDimensions Dimensions;
    };

    GameMode _gameMode;

    static GameLevels * Create(GameLevel gameLevel, GameDifficultyLevel difficultyLevel, GameAlgo gameAlgo);

    void CleanupLevelData();

    void InitializeGameDataWithLevel();

    void SetGameType(GameLevel gameLevel, GameDifficultyLevel difficultyLevel, GameAlgo gameAlgo);
    GameType GetGameType();

    void SetGameLevel(GameLevel gameLevel);
    int GetGameLevel();

    void SetDifficultyLevel(GameDifficultyLevel difficultyLevel);
    GameDifficultyLevel GetDifficultyLevel();
    string GetDifficultyLevelName();

    void SetGameAlgo(GameAlgo gameAlgo);
    GameAlgo GetGameAlgo();
    string GetGameMode();

    void SetGameDimensions(GameLevel gameLevel);
    GameDimensions GetGameDimensions();

    int GetPlayerCurrentScore();
    void UpdatePlayerCurrentScore(int deltaScore);
    void SetPlayerCurrentScore(int score);

    int GetPlayerBestScore();
    void SetPlayerBestScore(int score);

    // Saving SubSquareContainer data in UserDefault and retrieving that data:
    void SaveSubSquareContainerValues(string dataString);
    void RetrieveSubSquareContainerValues();
    void DeleteSubSquareContainerValues();

    int GetSubSquareContainerValue(Game2048Common::Vec2I containerIndex);
    bool GetSubSquareContainerActive(Game2048Common::Vec2I containerIndex);
    unsigned int GetSubSquareContainerInactiveMovesCount(Game2048Common::Vec2I containerIndex);

    SubSquareContainerDetails GetSubSquareContainerDetails();
    bool IsSubSquareContainerIndexValid(Game2048Common::Vec2I containerIndex);

    void CleanupSubSquareContainerData();

    map<int, map<int, int>> GetAtomTiles(); // 2D map
    void SetAtomTile(bool isActive, int x, int y);
    void DisableAtomTiles();

protected:
    GameType _gameType;
    PlayerData _playerData;
    map<int, map<int, int>> _atomTiles;

    SubSquareContainerDetails _subSquareContainerDetails;
    std::map<string, std::map<string, SubSquareContainer::SubSquareContainerData>> _subSquareContainerData;
};

#endif // GameLevels_hpp