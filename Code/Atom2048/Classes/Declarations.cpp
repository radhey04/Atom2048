// Created by Radhey Shyam Meena
// March 2017.

#include "Declarations.h"

// Type : String
// App release data:
std::string Declarations::_productId = "", Declarations::_productName = "MathLab"; // Missing;
vector<string> Declarations::_leagueNames = { "Bronze", "Silver", "Gold", "Platinum" };
vector<string> Declarations::_medalNames = { "Shoes", "Rope", "Axe", "Lightning", "Flag" };
vector<string> Declarations::_starTypes = { "Bronze", "Silver", "Gold" };
vector<string> Declarations::_praiseText = { "Good", "Excellent", "Awesome", "Spectacular" };
vector<string> Declarations::_levelText = { "Novice", "Beginner", "Skilled", "Expert", "Master", "Pro" };
vector<string> Declarations::_levelCongratulationText = { "This is an excellent start. I promote you.", "Doing very good. Keep it up.", "Great going. You are one step closer to being a master", "Wow! You are a master now. One more step to Pro", "Whoohoo! You are all Pro. Explore other topic now" };

string Declarations::_errorGenericMessage = "Something went wrong please try again in some time.",
Declarations::_errorConnectionTimedOutMessage = "Either connection to server timed out or information has not been received. Please check the same and try again.",
Declarations::_errorUnspecifiedErrorMessage = "Unspecified error occured.";

string Declarations::_registrationTitleText = "PERSONALIZE LEARNING EXPERIENCE FOR YOUR CHILD",
Declarations::_playerDefaultName = "Climber", Declarations::_logicRootsEmailDomain = "@logicroots.com";

// Const Pre/post-text names: to Apprend or prepend
const string Declarations::_currentLevelNameText = "CurrentLevel", Declarations::_levelNameText = "Level", Declarations::_nullNameText = "null";

#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
//Code for windows:
std::string Declarations::_AppID = ""; // Id for the windows Store App. eg: "9WZDNCRFHVJL" for OneNote
#elif( (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX) )
//Code for Mac:
std::string Declarations::_AppID = "1158780595"; // App store ID: eg: "1129179941" for Plus10.
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//Code for Android:
std::string Declarations::_AppID = "com.spark.atom2048"; // Id for the app on PlayStore : eg: "com.google.android.gm" for gmail.
#else
//Code otherwise:
std::string Declarations::_AppID = ""; // Default ID:
#endif

// Type: Int / Vector of int

// Type: Double or vector<double>

// Type : Float
const float Declarations::_animationFrameTime = 0.040, Declarations::_progressBarFillTime = 0.5;

// Type: Size
cocos2d::Size Declarations::_designSize = cocos2d::Size(1366, 768);

// Type: Color
Color3B Declarations::_DarkBrown = Color3B(56,34,7), Declarations::_CountAnimationColor = Color3B(255,255,51);
Color4B Declarations::_PopUpMaskColor = Color4B(0,0,0,191);

// Type: bool

// Structs
Declarations::IndexName Declarations::_indexName;
Declarations::TypeName Declarations::_typeName;

// Type : Map
std::map<std::string, std::map<std::string, float>>
// Practice Game:
Declarations::_Game2048Xml
;

Declarations::Declarations()
{
}

Declarations::~Declarations()
{
}
