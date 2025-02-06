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
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "[MySQL Error] " << e.what() << std::endl;
        return false;
    }
}