// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : 10-June 2017.

#include "Common.h"
#include "IAPHelper.h"
#include "cocos2d.h"
#include "AnalyticsHelper.h"
#include "AppEvents.h"
#include "Player.h"

USING_NS_CC;

IAPHelper * IAPHelper::_IAPHelper = new IAPHelper();
IAPHelper::IAPHelper()
{
}

IAPHelper::~IAPHelper()
{
}

IAPHelper * IAPHelper::GetInstance()
{
    if (!_IAPHelper)
    {
        _IAPHelper = new IAPHelper();
    }
    return _IAPHelper;
}

void IAPHelper::InitializeIAP()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::IAP::init();

   // vector<Product> products = sdkbox::IAP::getProducts();

    // Attaching a listener:
    sdkbox::IAP::setListener(new IAPListenerLocal());
#else

#endif


}

void IAPHelper::RestoreIAP()
{
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    log("IAPHelper:RestoreIAP - restoring IAP items from google play.");
    sdkbox::IAP::restore();
#else

#endif
}

IAPHelper::IAPItemName IAPHelper::GetIAPItemNames()
{
    return _IAPItemName;
}

void IAPHelper::SetCallbackForIAPItem(string IAPItemName, IAPOutcome iAPOutcome, function<void()>& callback)
{
    _IAPIListenerCallback[IAPItemName][iAPOutcome] = callback;
}

void IAPHelper::BuyIAPItem(string IAPItemName, int duration){
    log("IAPHelper: BuyIAPItem initialized for item :%s with duration = %d", IAPItemName.c_str(), duration);
#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))
    sdkbox::IAP::purchase(IAPItemName);
#endif

    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().InAppPurchase, IAPItemName, "Purchase Initiated", 1);
}

bool IAPHelper::IsPaid(string IAPItemName)
{
    // Check if the value of UserDefault key 'IAPItemName' is saved, if so return its value;
    // Otherwise, return false.
    bool isPaid = UserDefault::getInstance()->getBoolForKey(IAPItemName.c_str(), false);
    log("IAPHelper: IsPaid - %s is %d.", IAPItemName.c_str(), isPaid);
    return isPaid;
}

void IAPHelper::SetPaid(string IAPItemName, bool value)
{
    // Storing the value of paid status locally in UserDefaults:
    UserDefault::getInstance()->setBoolForKey(IAPItemName.c_str(), value);
    log("IAPHelper: SetPaid - %s = %d.", IAPItemName.c_str(), value);
    // Player Purchases gets refreshed.
    Player::GetInstance()->RefreshPurchases();
}

void IAPHelper::TriggerCallbackForIAPItem(string IAPItemName, IAPOutcome iAPOutcome)
{
    if ((_IAPIListenerCallback.find(IAPItemName) != _IAPIListenerCallback.end()) && (_IAPIListenerCallback[IAPItemName].find(iAPOutcome) != _IAPIListenerCallback[IAPItemName].end()))
    {
        _IAPIListenerCallback[IAPItemName][iAPOutcome]();
    }
}

vector<IAPHelper::IAPProduct> IAPHelper::GetIAPProducts()
{
    /* Function incomplete : TBD */
    return vector<IAPHelper::IAPProduct>();
}

#if ((CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_IOS))

IAPListenerLocal::IAPListenerLocal()
{
}

IAPListenerLocal::~IAPListenerLocal()
{
}

void IAPListenerLocal::onInitialized(bool ok) {
    cocos2d::log("IAP: IAPListenerLocal::onInitialized. Status = %d", ok);
}
void IAPListenerLocal::onSuccess(sdkbox::Product const& p) {
    string remarks = "'In app purchase from " + CommonFunc::GetDeviceName() + "'";
    log("IAP:: Success. For %s - %s.", p.name.c_str(), remarks.c_str());
    // Saving the successful IAP purchase transaction:
    IAPHelper::GetInstance()->SetPaid(p.name, true);

    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().InAppPurchase, p.name, "Purchase Success", 1);
    IAPHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, IAPHelper::IAPOutcome::Success);
}

void IAPListenerLocal::onFailure(sdkbox::Product const& p, const std::string &msg) {
    log("IAP:: Failed - for %s - %s", p.name.c_str(), msg.c_str());
    string remarks = "'In app purchase FAIL from " + CommonFunc::GetDeviceName() + ". " + msg + "'";
    string pfEventLabel = "Purchase Failure: " + p.name + " : " + CommonFunc::GetDeviceName() + " : " + msg;
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().InAppPurchase, p.name, pfEventLabel, 1);
    IAPHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, IAPHelper::IAPOutcome::Failure);
}

void IAPListenerLocal::onCanceled(sdkbox::Product const& p) {
    string remarks = "'In app purchase Canceled from " + CommonFunc::GetDeviceName() + ".'";
    log("IAP:: Canceled. for %s - %s", p.name.c_str(), remarks.c_str());
    string pcEventLabel = "Purchase Canceled: " + p.name + " : " + CommonFunc::GetDeviceName();
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().InAppPurchase, p.name, pcEventLabel, 1);
    IAPHelper::GetInstance()->TriggerCallbackForIAPItem(p.name, IAPHelper::IAPOutcome::Canceled);
}

void IAPListenerLocal::onRestored(sdkbox::Product const& p) {
    log("IAP:: Restored for %s", p.name.c_str());
    IAPHelper::GetInstance()->SetPaid(p.name, true);
}

void IAPListenerLocal::onProductRequestSuccess(std::vector<sdkbox::Product> const &products) {
    log("IAP:: IAPListenerLocal::onProductRequestSuccess");
    for (int prodInd = 0; prodInd < products.size(); prodInd++) {
        log("IAP:: Product Request Success : For %s.", products[prodInd].name.c_str());
    }
}

void IAPListenerLocal::onProductRequestFailure(const std::string &msg) {
    log("IAP:: Product Request Failed - %s", msg.c_str());
}

void IAPListenerLocal::onRestoreComplete(bool ok, const std::string &msg) {
    log("IAP:: onRestoreComplete. %s", msg.c_str());
}

#else

#endif
