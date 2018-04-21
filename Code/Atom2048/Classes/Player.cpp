/*-------------------------------------------------------------------------
Created by Radhey Shyam Meena (radhey04@gmail.com)
Date : May 2017.
@ Logicroots Pvt Ltd.
-------------------------------------------------------------------------*/
#include <algorithm>
#include "Player.h"
#include "cocos2d.h"
#include "IAPHelper.h"

USING_NS_CC;

Player * Player::_player = new Player();

Player::Player()
{
    this->_diffusersCount = 0;
    // Learning: Don't use UserDefault for accessing saved data in a constructor of a class.
    //bool dummyBool = false;// UserDefault::getInstance()->getBoolForKey("dummyBool", false);
    //if (dummyBool)
    //{
    //    log("dummyBool was true");
    //}
    //else {
    //    log("dummyBool was false");
    //}
}

Player::~Player()
{
}

Player * Player::GetInstance()
{
    if (!_player) {
        _player = new Player();
    }
    return _player;
}

void Player::Initialize()
{
    // Initializing Social Media shares count:
    this->GetTotalSocialMediaShares();

    //TBD : add IAP or some prefix at the beginning of IAP Item names to save data in local Storage.
    this->RefreshPurchases();

    // Reading the Diffusers count :
    this->_diffusersCount = UserDefault::getInstance()->getIntegerForKey("AtomDiffuserCount", 0);
}

bool Player::IsPaidFor(string IAPItemName)
{
    if (this->_isPaid.find(IAPItemName) != this->_isPaid.end())
    {
        return this->_isPaid[IAPItemName];
    }

    return false;
}

void Player::RefreshPurchases()
{
    this->_isPaid[IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds] = IAPHelper::GetInstance()->IsPaid(IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds);
    this->_isPaid[IAPHelper::GetInstance()->GetIAPItemNames().UnlockNormalMode] = IAPHelper::GetInstance()->IsPaid(IAPHelper::GetInstance()->GetIAPItemNames().UnlockNormalMode);

    // IAP items isPaid values for player refreshed:
    for (map<string, bool>::iterator iterator = this->_isPaid.begin(); iterator != this->_isPaid.end(); iterator++)
    {
        log("Player::RefreshPurchases : (PayItem, IsPaid) : (%s, %d)", iterator->first.c_str(), iterator->second);
    }
}

void Player::SetTotalSocialMediaShares(int totalSocialMediaShares)
{
    _totalSocialMediaShares = totalSocialMediaShares;
    UserDefault::getInstance()->setIntegerForKey(TotalSocialMediaSharesKey.c_str(), _totalSocialMediaShares);
}

void Player::UpdateTotalSocialMediaShares(int deltaShares)
{
    _totalSocialMediaShares += deltaShares;
    UserDefault::getInstance()->setIntegerForKey(TotalSocialMediaSharesKey.c_str(), _totalSocialMediaShares);
}

int Player::GetTotalSocialMediaShares()
{
    _totalSocialMediaShares = UserDefault::getInstance()->getIntegerForKey(TotalSocialMediaSharesKey.c_str(), 0);
    return _totalSocialMediaShares;
}

void Player::SetDiffusersCount(int count)
{
    if (count < 0) {
        return;
    }
    this->_diffusersCount = count;

    UserDefault::getInstance()->setIntegerForKey("AtomDiffuserCount", this->_diffusersCount);
}

void Player::UpdateDiffusersCountBy(int delta)
{
    if ((this->_diffusersCount + delta) < 0) {
        return;
    }
    this->_diffusersCount += delta;
    UserDefault::getInstance()->setIntegerForKey("AtomDiffuserCount", this->_diffusersCount);
}

int Player::GetDiffusersCount()
{
    return this->_diffusersCount;
}


