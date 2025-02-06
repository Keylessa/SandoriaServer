// AuthManager.cpp

#include "AuthManager.h"

bool AuthManager::authenticateUser(const std::string& username, const std::string& password) {
    Database db(MYSQL_DB_ACCOUNT);
    auto pstmt = db.prepareStatement("SELECT * FROM users WHERE username = ? AND password = ?");
    pstmt->setString(1, username);
    pstmt->setString(2, password);
    auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());
    return res->next();
}