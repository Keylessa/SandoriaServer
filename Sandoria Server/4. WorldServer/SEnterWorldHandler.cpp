#include "SEnterWorldHandler.h"
#include <iostream>

SEnterWorldHandler::SEnterWorldHandler() {}
SEnterWorldHandler::~SEnterWorldHandler() {}

bool SEnterWorldHandler::EnterWorld(int CharacterID)
{
    std::cout << "Character " << CharacterID << " has entered the world." << std::endl;
    return true;
}
