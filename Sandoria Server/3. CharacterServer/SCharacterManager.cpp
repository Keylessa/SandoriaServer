#include "SCharacterManager.h"
#include "../6. Database/SDatabaseManager.h"
#include <iostream>

SCharacterManager::SCharacterManager() {}
SCharacterManager::~SCharacterManager() {}

std::vector<SCharacterData> SCharacterManager::LoadCharacters(int AccountID)
{
    std::vector<SCharacterData> Characters;

    // Simulăm încărcarea din baza de date
    SCharacterData Char1 = { 1, "Arthas", 10, 2, 1, 100.0f, 200.0f, 0.0f };
    SCharacterData Char2 = { 2, "Sylvanas", 12, 3, 2, 150.0f, 250.0f, 0.0f };

    Characters.push_back(Char1);
    Characters.push_back(Char2);

    return Characters;
}

bool SCharacterManager::CreateCharacter(int AccountID, const std::string& Name, int Class, int Faction)
{
    std::cout << "Creating character: " << Name << " for account " << AccountID << std::endl;
    return true; // Simulăm succesul
}
