// Class : GameSound
// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : Jan 2018.

// sound credits : https://www.zapsplat.com/

/*
    Class to handle sounds in the game.
    Eg. Button click sounds.
*/

#include "GameSound.h"

using namespace std;

GameSound * GameSound::_sound = new GameSound();

GameSound::GameSound()
{
}

GameSound::~GameSound()
{
}

GameSound * GameSound::GetInstance()
{
    if (!GameSound::_sound) {
        GameSound::_sound = new GameSound();
    }
    return GameSound::_sound;
}

GameSound::SoundName GameSound::GetSoundName()
{
    return this->_soundName;
}

void GameSound::PlaySound(string name)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect((this->_soundPath + name).c_str());
}
