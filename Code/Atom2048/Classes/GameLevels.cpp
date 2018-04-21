// Created by Radhey Shyam Meena
// March 2017.

#include "GameLevels.h"
#include "Game2048Scene.h"

GameLevels::GameLevels()
{
    this->_gameType = {};
    this->_playerData = {};
    this->_subSquareContainerDetails = {};
    this->_subSquareContainerData.clear();
}

GameLevels::~GameLevels()
{
}

GameLevels * GameLevels::Create(GameLevels::GameLevel gameLevel, GameLevels::GameDifficultyLevel difficultyLevel, GameLevels::GameAlgo gameAlgo) {
    GameLevels * gameLevels = new GameLevels();
    gameLevels->SetGameType(gameLevel, difficultyLevel, gameAlgo);
    gameLevels->InitializeGameDataWithLevel();
    return gameLevels;
}

void GameLevels::CleanupLevelData()
{
    this->_gameType = {};
    this->_playerData = {};
    this->_subSquareContainerDetails = {};
}

void GameLevels::InitializeGameDataWithLevel()
{
    // Player Data:
    this->_playerData.CurrentScore = UserDefault::getInstance()->getIntegerForKey(("CurrentScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), 0);
    this->_playerData.BestScore = UserDefault::getInstance()->getIntegerForKey(("BestScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), 0);

    // SubSquareContainer Details setting:
    Size mainSquareContainerSize = Size(668.0, 668.0);
    this->_subSquareContainerDetails.ContainerMargin = Vec2(10.0, 10.0);
    // SubSquareContainer Details:
    this->_subSquareContainerDetails.ContainerSize = Size((mainSquareContainerSize.width - (this->GetGameDimensions().X + 1) * this->_subSquareContainerDetails.ContainerMargin.x) / (this->GetGameDimensions().X), (mainSquareContainerSize.height - (this->GetGameDimensions().Y + 1) * this->_subSquareContainerDetails.ContainerMargin.y) / (this->GetGameDimensions().Y));
    

    switch (this->GetGameLevel())
    {
    case GameLevels::GameLevel::Two:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().XExtraLarge;
        break;

    case GameLevels::GameLevel::Three:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().XExtraLarge;
        break;

    case GameLevels::GameLevel::Four:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().ExtraLarge;
        break;

    case GameLevels::GameLevel::Five:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Large;
        break;

    case GameLevels::GameLevel::Six:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Medium;
        break;

    case GameLevels::GameLevel::Seven:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Medium;
        break;

    case GameLevels::GameLevel::Eight:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Small;
        break;

    case GameLevels::GameLevel::Nine:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Small;
        break;

    case GameLevels::GameLevel::Ten:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().Regular;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Small;
        break;

    default:
        // SubSquareContainer Details:
        this->_subSquareContainerDetails.FontFace = Game2048Common::_gameTheme->GetFontFace().SemiBold;
        this->_subSquareContainerDetails.FontSize = Game2048Common::_gameTheme->GetFontSize().Large;
        break;
    }
}

void GameLevels::SetGameType(GameLevels::GameLevel gameLevel, GameLevels::GameDifficultyLevel difficultyLevel, GameLevels::GameAlgo gameAlgo)
{
    // This function should eventually set following parameters:
    // GameLevel, GameDifficulty, GameDifficultyLevelName, GameAlgo and GameMode, GameDimensions.
    this->SetGameLevel(gameLevel);
    this->SetDifficultyLevel(difficultyLevel);
    this->SetGameAlgo(gameAlgo);
    this->SetGameDimensions(gameLevel);
}

GameLevels::GameType GameLevels::GetGameType()
{
    return  this->_gameType;
}

void GameLevels::SetGameLevel(GameLevel gameLevel)
{
    this->_gameType.GameLevel = gameLevel;
}

int GameLevels::GetGameLevel() {
    return this->_gameType.GameLevel;
}

void GameLevels::SetGameDimensions(GameLevel gameLevel)
{
    this->_gameType.Dimensions.X = gameLevel;
    this->_gameType.Dimensions.Y = gameLevel;
}

GameLevels::GameDimensions GameLevels::GetGameDimensions()
{
    return GameDimensions(this->_gameType.Dimensions);
}

void GameLevels::SetDifficultyLevel(GameLevels::GameDifficultyLevel difficultyLevel)
{
    this->_gameType.DifficultyLevel = difficultyLevel;

    switch (difficultyLevel)
    {
    case GameLevels::Easy:
        this->_gameType.DifficultyLevelName = "Easy";
        break;
    case GameLevels::Medium:
        this->_gameType.DifficultyLevelName = "Medium";
        break;
    case GameLevels::Hard:
        this->_gameType.DifficultyLevelName = "Hard";
        break;
    default:
        break;
    }
}

GameLevels::GameDifficultyLevel GameLevels::GetDifficultyLevel() {
    return this->_gameType.DifficultyLevel;
}

string GameLevels::GetDifficultyLevelName() {
    return this->_gameType.DifficultyLevelName;
}

void GameLevels::SetGameAlgo(GameLevels::GameAlgo gameAlgo)
{
    this->_gameType.Algo = gameAlgo;

    switch (gameAlgo)
    {
    case GameLevels::Normal:
        this->_gameType.Mode = GameLevels::_gameMode.Normal;
        break;
    case GameLevels::OneInactive:
        this->_gameType.Mode = GameLevels::_gameMode.Normal;
        break;
    case GameLevels::Atom:
        this->_gameType.Mode = GameLevels::_gameMode.Atom;
        break;
    default:
        break;
    }
}

GameLevels::GameAlgo GameLevels::GetGameAlgo()
{
    return this->_gameType.Algo;
}

string GameLevels::GetGameMode()
{
    return this->_gameType.Mode;
}

int GameLevels::GetPlayerCurrentScore()
{
    return this->_playerData.CurrentScore;
}

void GameLevels::UpdatePlayerCurrentScore(int score)
{
    if (score > this->_playerData.CurrentScore)
    {
        this->_playerData.CurrentScore = score;
    }
    else {
        return;
    }
    //this->_playerData.CurrentScore += score;
    UserDefault::getInstance()->setIntegerForKey(("CurrentScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), this->_playerData.CurrentScore);
    if (this->_playerData.CurrentScore > this->_playerData.BestScore)
    {
        this->SetPlayerBestScore(this->_playerData.CurrentScore);
    }
}

void GameLevels::SetPlayerCurrentScore(int score)
{
    this->_playerData.CurrentScore = score;

    if (score > 0)
    {
        UserDefault::getInstance()->setIntegerForKey(("CurrentScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), score
        );
    }
    else {
        UserDefault::getInstance()->deleteValueForKey(("CurrentScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str());
    }
}

int GameLevels::GetPlayerBestScore()
{
    return this->_playerData.BestScore;
}

void GameLevels::SetPlayerBestScore(int score)
{
    this->_playerData.BestScore = score;
    UserDefault::getInstance()->setIntegerForKey(("BestScore-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), this->_playerData.BestScore);
}

void GameLevels::SaveSubSquareContainerValues(string dataString)
{
    // dataString format: "xIndex-yIndex:Value-Active-InactiveMoveCounts;"
    UserDefault::getInstance()->setStringForKey(("SubSquareContainerValue-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), dataString);
}

void GameLevels::RetrieveSubSquareContainerValues()
{
    // dataString format: "xIndex-yIndex:Value-Active-InactiveMoveCounts;xIndex-yIndex:Value-Active-InactiveMoveCounts;"
    string dataString = UserDefault::getInstance()->getStringForKey(("SubSquareContainerValue-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str(), "");
    if (dataString.compare("") != 0)
    {
        vector<string> containerData = CommonFunc::SplitString(dataString, ';', 0);
        if (containerData.size() > 0)
        {
            for (int containerIndex = 0; containerIndex < (int)containerData.size(); containerIndex++)
            {
                vector<string> containerIndexValueString = CommonFunc::SplitString(containerData[containerIndex], ':', 0);
                if (containerIndexValueString.size() > 1)
                {
                    vector<string> containerIndexString = CommonFunc::SplitString(containerIndexValueString[0], '-', 0);
                    vector<string> containerValueString = CommonFunc::SplitString(containerIndexValueString[1], '-', 0);
                    // Validate for the number of elements in the vector everyTime you update this function.
                    if ((containerIndexString.size() > 1) && (containerValueString.size() > 2))
                    {
                        this->_subSquareContainerData[containerIndexString[0]][containerIndexString[1]].Value = CommonFunc::StringToInt(containerValueString[0]);
                        this->_subSquareContainerData[containerIndexString[0]][containerIndexString[1]].IsActive = CommonFunc::StringToInt(containerValueString[1]);
                        this->_subSquareContainerData[containerIndexString[0]][containerIndexString[1]].InactiveMovesCount = CommonFunc::StringToInt(containerValueString[2]);
                    }
                }
            }
        }
    }
    else {
        this->_subSquareContainerData = {};
    }
}

void GameLevels::DeleteSubSquareContainerValues()
{
    // Delete the dataString for the level data:
    UserDefault::getInstance()->deleteValueForKey(("SubSquareContainerValue-" + CommonFunc::IntToString(this->GetGameLevel()) + CommonFunc::IntToString(this->GetDifficultyLevel()) + this->GetGameMode()).c_str());
}

int GameLevels::GetSubSquareContainerValue(Game2048Common::Vec2I containerIndex)
{
    if (this->IsSubSquareContainerIndexValid(containerIndex))
    {
        return this->_subSquareContainerData[CommonFunc::IntToString(containerIndex.X)][CommonFunc::IntToString(containerIndex.Y)].Value;
    }
    return 0;
}

bool GameLevels::GetSubSquareContainerActive(Game2048Common::Vec2I containerIndex)
{
    if (this->IsSubSquareContainerIndexValid(containerIndex))
    {
        return this->_subSquareContainerData[CommonFunc::IntToString(containerIndex.X)][CommonFunc::IntToString(containerIndex.Y)].IsActive;
    }
    return true;
}

unsigned int GameLevels::GetSubSquareContainerInactiveMovesCount(Game2048Common::Vec2I containerIndex)
{
    if (this->IsSubSquareContainerIndexValid(containerIndex))
    {
        return this->_subSquareContainerData[CommonFunc::IntToString(containerIndex.X)][CommonFunc::IntToString(containerIndex.Y)].InactiveMovesCount;
    }
    return 0;
}

GameLevels::SubSquareContainerDetails GameLevels::GetSubSquareContainerDetails()
{
    return this->_subSquareContainerDetails;
}

bool GameLevels::IsSubSquareContainerIndexValid(Game2048Common::Vec2I containerIndex)
{
    if ((this->_subSquareContainerData.find(CommonFunc::IntToString(containerIndex.X)) != this->_subSquareContainerData.end()) &&
        (this->_subSquareContainerData[CommonFunc::IntToString(containerIndex.X)].find(CommonFunc::IntToString(containerIndex.Y)) != this->_subSquareContainerData[CommonFunc::IntToString(containerIndex.X)].end()))
    {
        return true;
    }
    return false;
}

void GameLevels::CleanupSubSquareContainerData()
{
    this->_subSquareContainerData.clear();
}

map<int, map<int, int>> GameLevels::GetAtomTiles()
{
    return _atomTiles;
}

void GameLevels::SetAtomTile(bool isActive, int x, int y)
{
    if (isActive) {
    }
    else {
        this->_atomTiles[x][y] = 1;
    }
}

void GameLevels::DisableAtomTiles()
{
    this->_atomTiles.clear();

}
