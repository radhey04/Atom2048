// Class : GameSound
// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : Jan 2018.

// sound credits : https://www.zapsplat.com/

/*
    Class to handle sounds in the game.
    Eg. Button click sounds.
*/

#ifndef GameSound_hpp
#define GameSound_hpp

#include <string>
#include "SimpleAudioEngine.h"

using namespace std;

class GameSound
{
public:
    GameSound();
    virtual ~GameSound();

    struct SoundName
    {
        string ButtonClick = "ButtonClick.mp3";
        string DisabledButtonClick = "DeniedSelectionClick.mp3";
        string ShortCelebration = "ShortCoinFall.mp3";
        string PopupAppears = "PopupAppears.mp3";
        string TileDisables = "PopupAppears.mp3";
        string TileEnables = "ShortCoinFall.mp3";
    };

    
    static GameSound * GetInstance();

    SoundName GetSoundName();

    void PlaySound(string name);

private:
    static GameSound * _sound;
    string _soundPath = "Assets/Sound/";
    SoundName _soundName;
};

#endif // GameSound_hpp

