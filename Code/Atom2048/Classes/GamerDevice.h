// Class : GamerDevice
// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : Jan 2018.

/*
    Class to interact with device's hardware.
    Eg. Vibration functionality.
*/

#ifndef GamerDevice_hpp
#define GamerDevice_hpp

#include <string>

using namespace std;

class GamerDevice
{
public:
    GamerDevice();
    ~GamerDevice();

    struct VibrationType {
        float Short = 0.1;
        float Medium = 0.4;
        float Long = 1.0;
    };

    static GamerDevice * GetInstance();

    void VibrateDevice(float duration);

    // Vibrate types :
    VibrationType GetVibrateType();

private:
    static GamerDevice * _gamerDevice;
    VibrationType _vibrationType;
};

#endif // GamerDevice_hpp