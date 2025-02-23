#pragma once
#include "../3. CharacterServer/SCharacterManager.h"

class SEnterWorldHandler
{
public:
    SEnterWorldHandler();
    ~SEnterWorldHandler();

    bool EnterWorld(int CharacterID);
};
