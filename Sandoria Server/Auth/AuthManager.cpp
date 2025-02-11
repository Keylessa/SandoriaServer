// AuthManager.cpp

#include "AuthManager.h"
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <vector>
#include <iostream>


std::string hashPassword(const std::string& password, const std::string& salt) {
    std::vector<unsigned char> hash(32); // 256-bit hash

    PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
        reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(),
        10000, EVP_sha256(), hash.size(), hash.data());

    std::string hashedPassword;
    for (unsigned char c : hash) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", c);
        hashedPassword += buffer;
    }

    return hashedPassword;
}

bool AuthManager::authenticateUser(const std::string& username, const std::string& password) {
    Database db(MYSQL_DB_ACCOUNT);
    auto pstmt = db.prepareStatement("SELECT password, salt FROM users WHERE username = ?");
    pstmt->setString(1, username);
    auto res = std::unique_ptr<sql::ResultSet>(pstmt->executeQuery());

    if (res->next()) {
        std::string storedHash = res->getString("password");
        std::string salt = res->getString("salt");

        // Calculăm hash-ul pentru parola introdusă de utilizator și îl comparăm cu cel stocat
        return hashPassword(password, salt) == storedHash;
    }
    return false;
}

int AuthManager::getAccountID(const std::string& username, const std::string& password)
{
    Database db("account");  // Conectează-te la baza de date `account`

    try {
        std::unique_ptr<sql::PreparedStatement> stmt(db.prepareStatement(
            "SELECT id, password, salt FROM users WHERE username = ? LIMIT 1"
        ));

        stmt->setString(1, username);

        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        if (res->next()) {
            std::string storedHash = res->getString("password");
            std::string salt = res->getString("salt");

            // ✅ Folosim aceeași metodă de hashing ca `authenticateUser()`
            if (hashPassword(password, salt) == storedHash) {
                return res->getInt("id");  // ✅ Returnăm `AccountID` dacă parola este corectă
            }
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL Error: " << e.what() << std::endl;
    }

    return -1;  // ❌ Dacă nu găsim contul sau parola e greșită, returnăm -1
}
