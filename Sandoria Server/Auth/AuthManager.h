// AuthManager.h - Gestionarea autentificării
#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "../Core/Database.h"

class AuthManager {
public:
    static bool authenticateUser(const std::string& username, const std::string& password);
};

#endif // AUTHMANAGER_H