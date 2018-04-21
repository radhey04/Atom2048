// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : March 2017.

#include "GameTheme.h"

GameTheme::GameTheme()
{
}

GameTheme::~GameTheme()
{
}

GameTheme::FontFace GameTheme::GetFontFace()
{
    return _fontFace;
}

GameTheme::FontSize GameTheme::GetFontSize()
{
    return _fontSize;
}

GameTheme::GameColors GameTheme::GetGameColors()
{
    return _gameColors;
}

GameTheme::ColorForNum GameTheme::GetColorForNum()
{
    return _colorForNum;
}