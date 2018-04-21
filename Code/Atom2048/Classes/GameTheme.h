// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : March 2017.

#ifndef GameTheme_hpp
#define GameTheme_hpp

#include "Common.h"

class GameTheme
{
public:
    struct FontFace
    {
        // Font face:
        std::string Regular = "OpenSans-Regular.ttf";
        std::string SemiBold = "OpenSans-Semibold.ttf";
        std::string Bold = "OpenSans-Bold.ttf";
        std::string ExtraBold = "OpenSans-ExtraBold.ttf";
        std::string Italic = "OpenSans-Italic.ttf";
        std::string BoldItalic = "OpenSans-BoldItalic.ttf";
        std::string SemiboldItalic = "OpenSans-SemiboldItalic.ttf";
    };

    struct FontSize
    {
        float ExtraSmall = 20.0;
        float Small = 25.0;
        float Medium = 32.0;
        float Large = 41.0;
        float ExtraLarge = 60.0;
        float XExtraLarge = 80.0;
    };

    struct GameColors {
        Color3B GameBaseTheme = Color3B(187, 173, 160);
        Color3B MainSquareContainerColor = Color3B(205, 193, 181);
        Color3B SubSquareContainerColor = Color3B(204, 192, 180);
        Color3B NewGameContainerColor = Color3B(142, 121, 100);
        Color3B TextColorDarkBrown = Color3B(142, 121, 100);
        Color3B ThemeTextWhite = Color3B(245,245,245);
        Color4B PopupColor = Color4B(0, 0, 0, 210);
        Color3B DisabledTile = Color3B(42, 49, 50);  //(81, 179, 236);
        Color3B TileText1 = Color3B(10,10,10);
        Color3B TileText2 = Color3B(245,245,245);
        Color3B HeavyButton = Color3B(5, 132, 242);
        Color3B LightButton = Color3B(52, 136, 140); // 33, 38, 36
        Color3B DiabledButton = Color3B(42, 49, 50);
    };

    struct ColorForNum {
        Color3B Num2 = Color3B(233, 223, 213);
        Color3B Num4 = Color3B(236, 224, 202);
        Color3B Num8 = Color3B(242, 176, 118);
        Color3B Num16 = Color3B(235, 152, 102);
        Color3B Num32 = Color3B(247, 121, 96);
        Color3B Num64 = Color3B(244, 95, 62);
        Color3B Num128 = Color3B(237, 207, 111);
        Color3B Num256 = Color3B(232, 205, 92);
        Color3B Num512 = Color3B(237, 213, 127);
        Color3B Num1024 = Color3B(236, 196, 62);
        Color3B Num2048 = Color3B(237, 196, 46);
        Color3B Num4096 = Color3B(240, 67, 61);
        Color3B Num8192 = Color3B(254, 30, 30);
        Color3B Num16384 = Color3B(251, 32, 28);
        Color3B Num32768 = Color3B(248, 29, 27);
        Color3B Num65536 = Color3B(232, 42, 42);
        Color3B Num131072 = Color3B(244, 32, 28);
    };

    GameTheme();
    ~GameTheme();

    // Getters:
    FontFace GetFontFace();
    FontSize GetFontSize();
    GameColors GetGameColors();
    ColorForNum GetColorForNum();

private:
    // Class Members:
    FontFace _fontFace;
    FontSize _fontSize;
    GameColors _gameColors;
    ColorForNum _colorForNum;
};

#endif // GameTheme_hpp