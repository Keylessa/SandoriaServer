#pragma once
#include <vector>
#include <string>

struct SCharacterData
{
    int CharacterID;
    std::string Name;
    int Level;
    int Class;
    int Faction;
    float PositionX;
    float PositionY;
    float PositionZ;
};

class SCharacterManager
{
public:
    SCharacterManager();
    ~SCharacterManager();

    std::vector<SCharacterData> LoadCharacters(int AccountID);
    bool CreateCharacter(int AccountID, const std::string& Name, int Class, int Faction);
};
