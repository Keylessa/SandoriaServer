///////////////////////////////////
// SLoginHandler.h
//////////////////////////////////

#pragma once
#include "SAuthManager.h"

class SLoginHandler
{
public:
    SLoginHandler(SAuthManager& AuthMgr);
    ~SLoginHandler();

    bool HandleLoginRequest(const std::string& Username, const std::string& Password);
private:
    SAuthManager& AuthManager;
};
