/*-------------------------------------------------------------------------
Created by Radhey Shyam Meena (radhey04@gmail.com)
Date : July 2017.
    -------------------------------------------------------------------------*/
#ifndef AppEvents_hpp
#define AppEvents_hpp

#include <string>
#include "Common.h"

using namespace std;

class AppEvents
{
public:
    AppEvents();
    ~AppEvents();
    
    // EventCategory
    struct EventCategory {
        string Ads = "Ads";
        string AtomDiffusers = "AtomDiffusers";
        string ExitGame = "ExitGame";
        string GPGLogin = "GPGLogin";
        string GameInstallation = "GameInstallation";
        string GameInteraction = "GameInteraction";
        string GameModeChange = "GameModeChange";
        string GameModeStarts = "GameModeStarts";
        string GameOver = "GameOver";
        string GamePlay = "GamePlay";
        string GameWon = "GameWon";
        string HomeInteraction = "HomeInteraction";
        string InAppPurchase = "InAppPurchase";
        string RateTheApp = "RateTheApp";
        string RestartGame = "RestartGame";
        string UserSession = "UserSession";
    };
    
    // EventAction :
    struct EventAction {
        string AdShown                    = "AdShown";
        string AgreesToRate               = "AgreesToRate";
        string AppToBackground            = "AppToBackground";
        string AppToForeground            = "AppToForeground";
        string AtomModeInitiated          = "AtomModeInitiated";
        string AtomModeStarts             = "AtomModeStarts";
        string BannerAd                   = "BannerAd";
        string DiffuserScreen             = "DiffuserScreen";
        string DiffuserUtilization        = "DiffuserUtilization";
        string DiffuserBuy                = "DiffuserBuy";
        string ExitGame                   = "ExitGame";
        string ExitedGame                 = "ExitedGame";
        string Failed                     = "Failed"; // Unclear
        string FirstRun                   = "FirstRun";
        string GameContinues              = "GameContinues";
        string GameOver                   = "GameOver";
        string GameWon                    = "GameWon";
        string ImageAd                    = "ImageAd";
        string Initiated                  = "Initiated"; // Unclear
        string Leaderboard                = "Leaderboard";
        string LinkSharing                = "LinkSharing";
        string ModeChange                 = "ModeChange";
        string NormalModeInitiated        = "NormalModeInitiated";
        string NormalModeStarts           = "NormalModeStarts";
        string NotAgreeToRate             = "NotAgreeToRate";
        string NotRestartedGame           = "NotRestartedGame";
        string PurchaseNormalMode         = "PurchaseNormalMode";
        string PurchaseNormalModeCanceled = "PurchaseNormalModeCanceled";
        string PurchaseNormalModeFailed   = "PurchaseNormalModeFailed";
        string PurchaseNormalModeSuccess  = "PurchaseNormalModeSuccess";
        string PurchaseRemoveAds          = "PurchaseRemoveAds";
        string PurchaseRemoveAdsCanceled  = "PurchaseRemoveAdsCanceled";
        string PurchaseRemoveAdsFailed    = "PurchaseRemoveAdsFailed";
        string PurchaseRemoveAdsSuccess   = "PurchaseRemoveAdsSuccess";
        string RateTheApp                 = "RateTheApp";
        string RemoveAds                  = "RemoveAds";
        string RestartGame                = "RestartGame";
        string RestartedGame              = "RestartedGame";
        string SessionStarts              = "SessionStarts";
        string SessionsCount              = "SessionsCount";
        string StaysInGame                = "StaysInGame";
        string Success                    = "Success"; // Unclear
        string TileDisabled               = "TileDisabled";
        string TileEnabled                = "TileEnabled";
        string TilesMerged                = "TilesMerged";
        string TryAgain                   = "TryAgain";
        string VideoAd                    = "VideoAd";
    };
    
    // EventLabel
    
    // Getters and Setters :
    static AppEvents * GetInstance();
    EventCategory GetEventCategory();
    EventAction GetEventAction();

protected:
    static AppEvents * _appEvents;
};

#endif // AppEvents_hpp