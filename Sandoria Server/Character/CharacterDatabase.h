// CharacterDatabase.h - Clasa pentru interacțiunea cu baza de date characters

#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "../Core/Config.h"
#include <memory>
#include <string>

class CharacterDatabase {
public:
    CharacterDatabase();
    ~CharacterDatabase();
    bool createCharacter(const std::string& accountName, const std::string& charName, const std::string& race, const std::string& charClass, const std::string& gender);

private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;
};
