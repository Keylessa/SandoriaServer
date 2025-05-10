///////////////////////////////////
// SandoriaServer.cpp (main file)
//////////////////////////////////
#include "1. ServerCore/SServerConnection.h"
#include "4. WorldServer/SWorldManager.h"
#include "5. Network/SNetworkHandler.h"
#include "6. Database/SDatabaseManager.h"
#include "8. Globals/Globals.h"
#include <iostream>

// Instanță globală pentru SDatabaseManager
SDatabaseManager* globalDatabaseManager = nullptr;

int main()
{
    std::cout << "Starting Sandoria Server..." << std::endl;

    SDatabaseManager dbManager;
    if (!dbManager.Initialize()) {
        std::cerr << "Failed to initialize database connections!" << std::endl;
        return -1;
    }
    globalDatabaseManager = &dbManager;

    SServerConnection ServerConnection;
    SNetworkHandler NetworkHandler;
    SWorldManager WorldManager;

    if (!ServerConnection.Initialize()) {
        std::cerr << "Failed to initialize server connection!" << std::endl;
        return -1;
    }
    if (!NetworkHandler.Initialize(7777)) {
        std::cerr << "Failed to initialize network handler!" << std::endl;
        return -1;
    }

    WorldManager.Initialize();

    while (true) {
        NetworkHandler.Update();  // Ascultă pachetele de la clienți
        WorldManager.Update();    // Sincronizează lumea
    }

    return 0;
}