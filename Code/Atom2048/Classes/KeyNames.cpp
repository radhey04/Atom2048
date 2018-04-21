// Created by Radhey Shyam Meena (radhey04@gmail.com)
// Date : July 2017.

#include "KeyNames.h"

KeyNames * KeyNames::_keyNames = new KeyNames();

KeyNames::KeyNames()
{
}

KeyNames::~KeyNames()
{
}

KeyNames * KeyNames::GetInstance()
{
    if (!_keyNames)
    {
        _keyNames = new KeyNames();
    }
    return _keyNames;
}

KeyNames::KeyName KeyNames::GetKeyName()
{
    return KeyName();
}
