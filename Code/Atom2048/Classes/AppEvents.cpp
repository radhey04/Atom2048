/*-------------------------------------------------------------------------
Created by Radhey Shyam Meena (radhey04@gmail.com)
Date : July 2017.
-------------------------------------------------------------------------*/
#include <algorithm>
#include "AppEvents.h"

AppEvents * AppEvents::_appEvents = new AppEvents();

AppEvents::AppEvents()
{
}

AppEvents::~AppEvents()
{
}

AppEvents * AppEvents::GetInstance(){
    if (!_appEvents) {
        _appEvents = new AppEvents();
    }
    return _appEvents;
}

AppEvents::EventCategory AppEvents::GetEventCategory(){
    return EventCategory();
}

AppEvents::EventAction AppEvents::GetEventAction(){
    // TBD : Research : What is better way for performance programming: 
    // 1. having _eventAction as class member of AppEvents;
    // 2. returning EventAction(), which I think creates a copy for each function call and then return it.
    return EventAction();
}