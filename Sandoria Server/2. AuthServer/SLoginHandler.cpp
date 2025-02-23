///////////////////////////////////
// SLoginHandler.cpp
//////////////////////////////////

#include "SLoginHandler.h"
#include <iostream>

SLoginHandler::SLoginHandler(SAuthManager& AuthMgr) : AuthManager(AuthMgr) {}

SLoginHandler::~SLoginHandler() {}

bool SLoginHandler::HandleLoginRequest(const std::string& Username, const std::string& Password)
{
    return AuthManager.VerifyLogin(Username, Password);
}
