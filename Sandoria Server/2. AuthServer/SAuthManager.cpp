#include "SAuthManager.h"
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/sha.h>

SAuthManager::SAuthManager(SDatabaseManager* dbManager)
    : DatabaseManager(dbManager)
{
}

SAuthManager::~SAuthManager() {}

std::string SAuthManager::ComputePBKDF2(const std::string& password, const std::string& salt, int iterations)
{
    const int keyLength = 64; // 64 octeți (rezultatul va avea 128 de caractere hex)
    unsigned char hash[keyLength];

    if (PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.length()),
        reinterpret_cast<const unsigned char*>(salt.c_str()), static_cast<int>(salt.length()),
        iterations, EVP_sha256(), keyLength, hash) != 1)
    {
        return "";
    }

    std::ostringstream oss;
    for (int i = 0; i < keyLength; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

bool SAuthManager::VerifyLogin(const std::string& Username, const std::string& Password)
{
    if (!DatabaseManager) {
        std::cerr << "Eroare: DatabaseManager este nullptr!" << std::endl;
        return false;
    }

    std::shared_ptr<sql::Connection> conn = DatabaseManager->GetAuthConnection();
    if (!conn) {
        std::cerr << "Conexiunea la baza de date Auth nu este disponibilă." << std::endl;
        return false;
    }

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT password, salt FROM account WHERE username = ?"));
        pstmt->setString(1, Username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            std::string storedHash = res->getString("password");
            std::string salt = res->getString("salt");
            std::string computedHash = ComputePBKDF2(Password, salt, 10000);
            if (computedHash == storedHash) {
                std::cout << "Login successful for: " << Username << std::endl;
                return true;
            }
            else {
                std::cout << "Login failed for: " << Username << std::endl;
                return false;
            }
        }
        else {
            std::cout << "Contul nu există pentru: " << Username << std::endl;
            return false;
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in VerifyLogin: " << e.what() << std::endl;
        return false;
    }
}
