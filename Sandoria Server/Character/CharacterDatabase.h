// CharacterDatabase.h - Clasa pentru interacțiunea cu baza de date characters

#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "../Core/Config.h"
#include <memory>
#include "../VectorRotator.h"
#include <vector>
#include <string>

struct CharacterDataList
{
    std::string Name;
    std::string Level;
    int Class;
    int Race;
};

struct CharacterData {
    // Date de bază
    std::string Name;
    uint8_t Level;
    uint32_t CharacterID;
    uint32_t OwnerID;   
    uint8_t Faction;
    uint8_t Race;
    uint8_t Class;
    uint8_t Gender;    
    uint32_t XP;
    uint32_t Money;
    FVector SpawnLocation;
    FRotator SpawnRotation;
    uint16_t MapID;
    bool IsOnline;
    std::string Appearance;

    // Atribute și statistici
    uint32_t MaxHealth;
    uint32_t MaxPower[7]; // Puteri multiple (mana, rage, etc.)
    uint32_t Strength;
    uint32_t Agility;
    uint32_t Stamina;
    uint32_t Intellect;
    uint32_t Spirit;
    uint32_t Armor;
    uint32_t Resistances[6]; // Holy, Fire, Nature, Frost, Shadow, Arcane
    float BlockChance;
    float DodgeChance;
    float ParryChance;
    float CritChance;
    float RangedCritChance;
    float SpellCritChance;
    uint32_t AttackPower;
    uint32_t RangedAttackPower;
    uint32_t SpellPower;
    uint32_t Resilience;
};

class CharacterDatabase {
public:
    CharacterDatabase();
    ~CharacterDatabase();
    bool createCharacter(int accountID, const std::string& name, int faction, int race, int charClass, int gender);
    void getClassStats(int& charClass, int& strength, int& agility, int& stamina, int& intellect, int& spirit,
        int& armor, int& resHoly, int& resFire, int& resNature, int& resFrost, int& resShadow, int& resArcane,
        float& blockPct, float& dodgePct, float& parryPct, float& critPct, float& rangedCritPct, float& spellCritPct,
        int& attackPower, int& rangedAttackPower, int& spellPower, int& resilience);

    std::vector<CharacterDataList> getCharacters(const std::string& accountName);

    bool doesCharacterExist(const std::string& charName);

    CharacterData LoadCharacter(uint32_t CharacterID);

private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;
};
