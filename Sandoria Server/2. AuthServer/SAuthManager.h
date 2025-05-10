#pragma once
#include <string>
#include "../6. Database/SDatabaseManager.h"

struct SAccountData {
    int AccountID;
    std::string Username;
    std::string PasswordHash;
    std::string Salt;
    std::string Email;
};

class SAuthManager {
public:
    // Constructorul principal
    SAuthManager(SDatabaseManager* dbManager);
    ~SAuthManager();

    bool VerifyLogin(const std::string& Username, const std::string& Password);

private:
    SDatabaseManager* DatabaseManager;
    // Calculează hash-ul PBKDF2 cu SHA256 (folosind OpenSSL)
    std::string ComputePBKDF2(const std::string& password, const std::string& salt, int iterations);
};
