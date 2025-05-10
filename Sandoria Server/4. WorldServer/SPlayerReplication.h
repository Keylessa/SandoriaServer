#pragma once
#include <unordered_map>

struct SPlayerPosition
{
    int PlayerID;
    float X, Y, Z;
};

class SPlayerReplication
{
public:
    SPlayerReplication();
    ~SPlayerReplication();

    void UpdatePlayerPosition(int PlayerID, float X, float Y, float Z);
    std::unordered_map<int, SPlayerPosition> GetAllPositions();
};
