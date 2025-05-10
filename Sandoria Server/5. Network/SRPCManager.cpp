#include "SRPCManager.h"

SRPCManager::SRPCManager() {}
SRPCManager::~SRPCManager() {}

void SRPCManager::CallRPC(int rpcID, const std::string& parameters)
{
    switch (rpcID)
    {
    case 1: // Exemplu pentru un RPC de login
        std::cout << "Calling login RPC with parameters: " << parameters << std::endl;
        // Aici adăugi logica pentru a procesa login-ul
        break;
        // Adaugă mai multe cazuri pentru alte RPC-uri
    default:
        std::cerr << "Unknown RPC ID!" << std::endl;
        break;
    }
}
