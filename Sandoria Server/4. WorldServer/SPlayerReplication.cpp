#include "SPlayerReplication.h"
#include <iostream>

std::unordered_map<int, SPlayerPosition> PlayerPositions;

SPlayerReplication::SPlayerReplication() {}
SPlayerReplication::~SPlayerReplication() {}

void SPlayerReplication::UpdatePlayerPosition(int PlayerID, float X, float Y, float Z)
{
    PlayerPositions[PlayerID] = { PlayerID, X, Y, Z };
    std::cout << "Updated position for Player " << PlayerID << ": " << X << ", " << Y << ", " << Z << std::endl;
}

std::unordered_map<int, SPlayerPosition> SPlayerReplication::GetAllPositions()
{
    return PlayerPositions;
}
