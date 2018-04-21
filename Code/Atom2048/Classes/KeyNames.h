// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : July 2017.

#ifndef KeyNames_hpp
#define KeyNames_hpp

#include <string>

using namespace std;

class KeyNames
{
public:
    KeyNames();
    ~KeyNames();

    struct KeyName
    {
        string FirstRun = "FirstRun";
        string SessionCount = "SessionCount";
    };

    // Getters / Setters:
    static KeyNames * GetInstance();
    KeyName GetKeyName();

private:
    static KeyNames * _keyNames;
    
};

#endif // KeyNames_hpp