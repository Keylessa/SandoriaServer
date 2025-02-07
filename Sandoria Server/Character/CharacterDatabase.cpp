#include "CharacterDatabase.h"
#include <iostream>

CharacterDatabase::CharacterDatabase() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conn = std::unique_ptr<sql::Connection>(driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS));
        conn->setSchema(MYSQL_DB_CHARACTER);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
    }
}

CharacterDatabase::~CharacterDatabase() {}

bool CharacterDatabase::createCharacter(const std::string& accountName, const std::string& charName, const std::string& race, const std::string& charClass, const std::string& gender) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO characters (accountName, charName, race, class, gender) VALUES (?, ?, ?, ?, ?)"));
        pstmt->setString(1, accountName);
        pstmt->setString(2, charName);
        pstmt->setString(3, race);
        pstmt->setString(4, charClass);
        pstmt->setString(5, gender);
        pstmt->executeUpdate();

        std::cout << "Data inserată în baza de date cu succes!" << std::endl;  // Log pentru succes
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
        return false;
    }
}

std::vector<CharacterData> CharacterDatabase::getCharacters(const std::string& accountName) {
    std::vector<CharacterData> characters;
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT charName, class, race FROM characters WHERE accountName = ?"));
        pstmt->setString(1, accountName);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            characters.push_back({ res->getString("charName"), res->getString("class"), res->getString("race") });
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
    }
    return characters;
}

bool CharacterDatabase::doesCharacterExist(const std::string& charName)
{
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) FROM characters WHERE charName = ?"));
        pstmt->setString(1, charName);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        res->next();
        return res->getInt(1) > 0;  // Returnează true dacă există cel puțin un rezultat
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
        return true;  // Dacă apare o eroare, presupunem că numele nu e disponibil pentru siguranță
    }
}
