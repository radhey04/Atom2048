// Created by Radhey Shyam Meena
// March 2017.

#ifndef SubSquareContainer_hpp
#define SubSquareContainer_hpp

#include "Game2048Common.h"
#include "Common.h"
#include "cocos2d.h"

class SubSquareContainer
{
public:
    struct SubSquareContainerData {
        unsigned int Value = 0;
        bool IsActive = true;
        unsigned int InactiveMovesCount = 0;
    };

    struct SubSquareContainerProperties {
        Color3B TextColor;
    };

    Game2048Common::Vec2I _containerIndex;

    SubSquareContainer();
    ~SubSquareContainer();

    static SubSquareContainer * CreateSprite(string assetName, Vec2 position, Game2048Common::Vec2I containerIndex, string assetIDName);
    //static SubSquareContainer * CreateLayer(Vec2 anchorPoint, Vec2 position, Size size, Color4B color, string assetIDName, Game2048Common::Vec2I containerIndex);

    Sprite * GetTile();

    void SetContainerColorForNum(int number);
    
    void SetContainerColorForNumText(int number);
    SubSquareContainerProperties GetContainerColorForNumText();

    static Color3B GetColorForNumer(int number);

    static string GetSpriteNameForNumber(int number);

    void SetSubSquareContainerIndex(Game2048Common::Vec2I containerIndex);
    Game2048Common::Vec2I GetSubSquareContainerIndex();

    void SetSubSquareContainerValue(unsigned int value);
    int GetSubSquareContainerValue();

    void SetSubSquareContainerActive(bool isActive);
    bool GetSubSquareContainerActive();

    void SetSubSquareContainerInactiveMovesCount(unsigned int inactiveMovesCount);
    void UpdateSubSquareContainerInactiveMovesCount(unsigned int inactiveMovesCount);
    unsigned int GetSubSquareContainerInactiveMovesCount();

    // SubSquareContainer * map : functions:
    static SubSquareContainer * GetSubSquareContainer(string tileName);

    void RemoveSubSquareContainer();
    void SetName(string name);

    static void CleanupAllSubSquareContaines();

    // Sprite functions:
    //void StopAllActions();
    //void DestroySelf();
	
    //void onExit();

private:
    Sprite * _tile;
    SubSquareContainerData _subSquareContainerData;
    SubSquareContainerProperties _subSquareContainerProperties;

    // Map to store all the SubSquareContainer created classes as pointers:
    static map<string, SubSquareContainer*> _allTiles;
};

#endif // !SubSquareContainer_hpp
