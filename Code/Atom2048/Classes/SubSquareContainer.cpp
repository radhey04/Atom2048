// Created by Radhey Shyam Meena
// March 2017.

#include "SubSquareContainer.h"
#include "GameSound.h"
#include "GamerDevice.h"
#include "Game2048Scene.h"

map<string, SubSquareContainer*> SubSquareContainer::_allTiles;

SubSquareContainer::SubSquareContainer()
{
}

SubSquareContainer::~SubSquareContainer()
{
}

SubSquareContainer * SubSquareContainer::CreateSprite(string assetName, Vec2 position, Game2048Common::Vec2I containerIndex, string assetIDName)
{
    // Adding this subSquareContainer to SubSquareContainer::_allTiles:
    if (SubSquareContainer::_allTiles.find(assetIDName) != SubSquareContainer::_allTiles.end())
    {
        // Delete it if a SubSquareContainer with same name already exists.
        delete SubSquareContainer::_allTiles.find(assetIDName)->second;
        SubSquareContainer::_allTiles.erase(assetIDName);
    }
    SubSquareContainer * subSquareContainer = new SubSquareContainer();
    SubSquareContainer::_allTiles[assetIDName] = subSquareContainer;

    subSquareContainer->_tile = CommonFunc::createSpriteFromCache(assetName, position.x, position.y, assetIDName);
    subSquareContainer->SetSubSquareContainerIndex(containerIndex);
    return subSquareContainer;
}

//SubSquareContainer * SubSquareContainer::CreateLayer(Vec2 anchorPoint, Vec2 position, Size size, Color4B color, string assetIDName, Game2048Common::Vec2I containerIndex)
//{
//    SubSquareContainer * subSquareContainer = (SubSquareContainer *)CommonFunc::CreateLayerColor(anchorPoint, position, size, color, assetIDName);
//    subSquareContainer->SetSubSquareContainerIndex(containerIndex);
//    subSquareContainer->InitializeContainer();
//    return subSquareContainer;
//}

Sprite * SubSquareContainer::GetTile()
{
    return this->_tile;
}

void SubSquareContainer::SetContainerColorForNum(int number)
{
    // Setting Text Color:
    //this->SetContainerColorForNumText(number);
    // version 1 
    // this->setColor(SubSquareContainer::GetColorForNumer(number));
    
    // Version 2 :
    CommonFunc::changeSpriteImage(this->_tile, SubSquareContainer::GetSpriteNameForNumber(this->_subSquareContainerData.Value));
}

void SubSquareContainer::SetContainerColorForNumText(int number)
{
    Color3B colorForNumText = Game2048Common::_gameTheme->GetGameColors().TextColorDarkBrown;
    switch (number)
    {
    case 2:
        colorForNumText = Game2048Common::_gameTheme->GetGameColors().TextColorDarkBrown;
        break;
    case 4:
        colorForNumText = Game2048Common::_gameTheme->GetGameColors().TextColorDarkBrown;
        break;
    default:
        colorForNumText = Color3B::WHITE;
        break;
    }
    this->_subSquareContainerProperties.TextColor = colorForNumText;
}

SubSquareContainer::SubSquareContainerProperties SubSquareContainer::GetContainerColorForNumText()
{
    return this->_subSquareContainerProperties;
}

Color3B SubSquareContainer::GetColorForNumer(int number) {
    Color3B colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num2;
    switch (number)
    {
    case 2:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num2;
        break;
    case 4:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num4;
        break;
    case 8:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num8;
        break;
    case 16:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num16;
        break;
    case 32:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num32;
        break;
    case 64:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num64;
        break;
    case 128:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num128;
        break;
    case 256:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num256;
        break;
    case 512:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num512;
        break;
    case 1024:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num1024;
        break;
    case 2048:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num2048;
        break;
    case 4096:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num4096;
        break;
    case 8192:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num8192;
        break;
    case 16384:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num16384;
        break;
    case 32768:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num32768;
        break;
    case 65536:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num65536;
        break;
    case 131072:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num131072;
        break;
    default:
        colorForNum = Game2048Common::_gameTheme->GetColorForNum().Num2;
        break;
    }

    return colorForNum;
}

string SubSquareContainer::GetSpriteNameForNumber(int number)
{
    // Example: if the number is 16, then returned string is "TileNum16.png";
    return string("TileNum" + CommonFunc::IntToString(number) + ".png");
}

void SubSquareContainer::SetSubSquareContainerIndex(Game2048Common::Vec2I containerIndex)
{
    this->_containerIndex = containerIndex;
}

Game2048Common::Vec2I SubSquareContainer::GetSubSquareContainerIndex()
{
    return this->_containerIndex;
}

void SubSquareContainer::SetSubSquareContainerValue(unsigned int value)
{
    this->_subSquareContainerData.Value = value;
}

int SubSquareContainer::GetSubSquareContainerValue()
{
    return this->_subSquareContainerData.Value;
}

void SubSquareContainer::SetSubSquareContainerActive(bool isActive)
{
    bool prevState = this->_subSquareContainerData.IsActive;
    this->_subSquareContainerData.IsActive = isActive;

    if (prevState && !isActive)
    {
        // Disabling the tile :
       // Game2048::_gameLevels->SetAtomTile(isActive, this->_containerIndex.X, this->_containerIndex.Y);

        GamerDevice::GetInstance()->VibrateDevice(GamerDevice::GetInstance()->GetVibrateType().Short);
        GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().TileDisables);
        CommonFunc::changeSpriteImage(this->_tile, "TileDisabled.png");
    }
    else if (isActive && !prevState) {
        // Tile just enabled :
       // Game2048::_gameLevels->SetAtomTile(isActive, this->_containerIndex.X, this->_containerIndex.Y);
        GamerDevice::GetInstance()->VibrateDevice(GamerDevice::GetInstance()->GetVibrateType().Short);
        GameSound::GetInstance()->PlaySound(GameSound::GetInstance()->GetSoundName().TileEnables);
        CommonFunc::changeSpriteImage(this->_tile, SubSquareContainer::GetSpriteNameForNumber(this->_subSquareContainerData.Value));
    }
}

bool SubSquareContainer::GetSubSquareContainerActive()
{
    return this->_subSquareContainerData.IsActive;
}

void SubSquareContainer::SetSubSquareContainerInactiveMovesCount(unsigned int inactiveMovesCount)
{
    this->_subSquareContainerData.InactiveMovesCount = inactiveMovesCount;
}

void SubSquareContainer::UpdateSubSquareContainerInactiveMovesCount(unsigned int inactiveMovesCount)
{
    this->_subSquareContainerData.InactiveMovesCount += inactiveMovesCount;
}

unsigned int SubSquareContainer::GetSubSquareContainerInactiveMovesCount()
{
    return this->_subSquareContainerData.InactiveMovesCount;
}

SubSquareContainer * SubSquareContainer::GetSubSquareContainer(string tileName)
{
    if (SubSquareContainer::_allTiles.find(tileName) != SubSquareContainer::_allTiles.end()) {
        return SubSquareContainer::_allTiles[tileName];
    }
    return nullptr;
}

void SubSquareContainer::RemoveSubSquareContainer()
{
    string tileName = _tile->getName();
    //_tile->removeFromParentAndCleanup(true);
    _tile->runAction(RemoveSelf::create());
    if (SubSquareContainer::_allTiles.find(tileName) != SubSquareContainer::_allTiles.end())
    {
        // Delete it if a SubSquareContainer with same name already exists.
        delete SubSquareContainer::_allTiles.find(tileName)->second;
        SubSquareContainer::_allTiles.erase(tileName);
    }
    this->~SubSquareContainer();
}

void SubSquareContainer::SetName(string name)
{
    string prevName = _tile->getName();
    if (SubSquareContainer::_allTiles.find(prevName) != SubSquareContainer::_allTiles.end())
    {
        SubSquareContainer::_allTiles[name] = SubSquareContainer::_allTiles.find(prevName)->second;

        // Deleting the prev name SubSquareContainer:
        SubSquareContainer::_allTiles[prevName] = nullptr;
        delete SubSquareContainer::_allTiles.find(prevName)->second;
        SubSquareContainer::_allTiles.erase(prevName);

        // Change the tile name as well:
        _tile->setName(name);
    }
}

void SubSquareContainer::CleanupAllSubSquareContaines()
{
    SubSquareContainer::_allTiles.clear();
}

//void SubSquareContainer::StopAllActions()
//{
//    this->Sprite::stopAllActions();
//}
//
//void SubSquareContainer::DestroySelf()
//{
//    delete this;
//}
//
//void SubSquareContainer::onExit()
//{
//    Sprite::onExit();
//}