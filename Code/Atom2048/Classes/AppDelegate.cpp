// Created by Radhey Shyam Meena
// March 2017.

#include "AppDelegate.h"
#include "Game2048Scene.h"
#include "AdHelper.h"
#include "IAPHelper.h"
#include "SdkboxPlayHelper.h"
#include "Player.h"
#include "AnalyticsHelper.h"
#include "AppEvents.h"

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
static cocos2d::Size designResolutionSize = cocos2d::Size(768, 1366); // cocos2d::Size(1366, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(320, 480); // cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(768, 1024); // cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1536, 2048); //cocos2d::Size(2048, 1536);
#else
static cocos2d::Size designResolutionSize = cocos2d::Size(2417, 1366);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);
#endif

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Game2048", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("Game2048");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    Size frameSize = glview->getFrameSize();
    float frameAspectRatio = frameSize.width / frameSize.height;

    // Set the design resolution
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.width / frameAspectRatio, ResolutionPolicy::NO_BORDER);
    // if the frame's height is larger than the height of medium size.
    director->setContentScaleFactor(1);
#else
    glview->setDesignResolutionSize(designResolutionSize.height * frameAspectRatio, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    // if the frame's height is larger than the height of medium size.
    director->setContentScaleFactor(1.0);
#endif

    register_all_packages();

    // StartGoogle Analytics:
    AnalyticsHelper::GetInstance()->Initialize();

    // Initializing SdkboxPlayHelper (for leaderboards and achievements) :
    SdkboxPlayHelper::GetInstance()->InitializeSdkboxPlay();

    // Initializing the IAP plugin:
    IAPHelper::GetInstance()->InitializeIAP();
    // Restoring all the purchased items:
    IAPHelper::GetInstance()->RestoreIAP();

    // Initializing Player Details:
    Player::GetInstance()->Initialize();

    // Check at the beginning if RemoveAds IAP is purchased, if so, don't show the ads.
    if (!Player::GetInstance()->IsPaidFor(IAPHelper::GetInstance()->GetIAPItemNames().RemoveAds))
    {
        // Initializing the Ads:
        AdHelper::GetInstance()->InitializeAds();
    }

    // create a scene. it's an autorelease object
    auto scene = Game2048::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameInteraction, AppEvents::GetInstance()->GetEventAction().AppToBackground, "App Goes in Background", 1);

    Director::getInstance()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    // Event:
    AnalyticsHelper::GetInstance()->LogEvent(AppEvents::GetInstance()->GetEventCategory().GameInteraction, AppEvents::GetInstance()->GetEventAction().AppToForeground, "App to Foreground", 1);

    Director::getInstance()->startAnimation();
}
