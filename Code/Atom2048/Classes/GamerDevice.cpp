// Class : GamerDevice
// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : Jan 2018.

/*
    Class to interact with device's hardware.
    Eg. Vibration functionality.
*/

#include "GamerDevice.h"
#include "cocos2d.h"

GamerDevice * GamerDevice::_gamerDevice = new GamerDevice();

GamerDevice::GamerDevice()
{
}

GamerDevice::~GamerDevice()
{
}

GamerDevice * GamerDevice::GetInstance()
{
    if (!GamerDevice::_gamerDevice) {
        GamerDevice::_gamerDevice = new GamerDevice();
    }
    return GamerDevice::_gamerDevice;
}

void GamerDevice::VibrateDevice(float duration)
{
    cocos2d::Device::vibrate(duration);

}

GamerDevice::VibrationType GamerDevice::GetVibrateType()
{
    return this->_vibrationType;
}
