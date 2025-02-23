#pragma once
#include <string>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include "Config.h"

// Structură pentru configurația unei baze de date
struct DatabaseConfig {
    std::string Host;
    std::string User;
    std::string Password;
    std::string Database;
    unsigned int Port;
};

class SDatabaseManager {
public:
    SDatabaseManager();
    ~SDatabaseManager();

    // Inițializează conexiunile la bazele de date.
    bool Initialize(const std::string& configFilePath = "dbconfig.ini");

    // Funcții pentru a obține conexiunile.
    std::shared_ptr<sql::Connection> GetAuthConnection();
    std::shared_ptr<sql::Connection> GetCharactersConnection();
    std::shared_ptr<sql::Connection> GetWorldConnection();

private:
    // Realizează o conexiune pentru configurația dată.
    bool ConnectDatabase(const DatabaseConfig& config, std::shared_ptr<sql::Connection>& connection);

    DatabaseConfig AuthConfig;
    DatabaseConfig CharactersConfig;
    DatabaseConfig WorldConfig;

    std::shared_ptr<sql::Connection> AuthConnection;
    std::shared_ptr<sql::Connection> CharactersConnection;
    std::shared_ptr<sql::Connection> WorldConnection;

    // Driver-ul MySQL Connector/C++
    sql::Driver* Driver;
};
