/*-------------------------------------------------------------------------
Created by Radhey Shyam Meena (radhey04@gmail.com)
Date : May 2017.
@ Logicroots Pvt Ltd.
-------------------------------------------------------------------------*/
#ifndef Player_hpp
#define Player_hpp

#include <string>
#include "Common.h"
#include <map>

using namespace std;

class Player
{
public:
    // Constuctor / Destructor:
    Player();
    ~Player();

    const string TotalSocialMediaSharesKey = "TotalSocialMediaShares";

    static Player * GetInstance();

    void Initialize();

    // Setters / Getters :
    // IsPaid:
    bool IsPaidFor(string IAPItemName);

    void RefreshPurchases();

    // Social Media Shares accounting:
    void SetTotalSocialMediaShares(int totalSocialMediaShares);
    void UpdateTotalSocialMediaShares(int deltaShares = 1);
    int GetTotalSocialMediaShares();

    // Diffusers Count :
    void SetDiffusersCount(int count);
    void UpdateDiffusersCountBy(int delta);
    int GetDiffusersCount();

protected:
    // Class Members:
    static Player * _player;

    // TBD : Think about the proper structure of how all the IAP items should be checked in the constructor
    // and this means, if any IAP item added later should automatically be handled here, without any code update.
    map<string, bool> _isPaid;

    // Number of shares / sharings on Social media, by clicking on the "Share" button on the game screen.
    int _totalSocialMediaShares = 0, _sessionNumber, _diffusersCount = 0;
};

#endif // Player_hpp