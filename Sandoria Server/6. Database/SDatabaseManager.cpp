#include "SDatabaseManager.h"
#include <cppconn/exception.h>
#include <iostream>

SDatabaseManager::SDatabaseManager()
    : AuthConnection(nullptr), CharactersConnection(nullptr), WorldConnection(nullptr), Driver(nullptr)
{
    try {
        Driver = get_driver_instance();
    }
    catch (sql::SQLException& e) {
        std::cerr << "Eroare la obținerea driver-ului MySQL: " << e.what() << std::endl;
    }
}

SDatabaseManager::~SDatabaseManager()
{
    // Smart pointers se ocupă de eliberarea memoriei.
}

bool SDatabaseManager::Initialize(const std::string& /*configFilePath*/)
{
    // Setăm configurațiile folosind macro-urile din Config.h.
    AuthConfig = { MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB_ACCOUNT, 3307 };
    CharactersConfig = { MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB_CHARACTER, 3307 };
    WorldConfig = { MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB_WORLD, 3307 };

    if (!ConnectDatabase(AuthConfig, AuthConnection)) {
        std::cerr << "Eroare: Nu s-a putut conecta la baza de date Auth." << std::endl;
        return false;
    }
    if (!ConnectDatabase(CharactersConfig, CharactersConnection)) {
        std::cerr << "Eroare: Nu s-a putut conecta la baza de date Characters." << std::endl;
        return false;
    }
    if (!ConnectDatabase(WorldConfig, WorldConnection)) {
        std::cerr << "Eroare: Nu s-a putut conecta la baza de date World." << std::endl;
        return false;
    }

    std::cout << "Toate conexiunile la bazele de date au fost inițializate cu succes." << std::endl;
    return true;
}

bool SDatabaseManager::ConnectDatabase(const DatabaseConfig& config, std::shared_ptr<sql::Connection>& connection)
{
    if (!Driver) {
        std::cerr << "Driver-ul MySQL nu este disponibil." << std::endl;
        return false;
    }
    try {
        connection.reset(Driver->connect(config.Host, config.User, config.Password));
        connection->setSchema(config.Database);
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in ConnectDatabase: " << e.what() << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<sql::Connection> SDatabaseManager::GetAuthConnection() {
    return AuthConnection;
}

std::shared_ptr<sql::Connection> SDatabaseManager::GetCharactersConnection() {
    return CharactersConnection;
}

std::shared_ptr<sql::Connection> SDatabaseManager::GetWorldConnection() {
    return WorldConnection;
}
