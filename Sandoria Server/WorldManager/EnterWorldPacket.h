#pragma once
#include <cstdint>

struct EnterWorldPacket {
    uint32_t PlayerID;
    uint32_t CharacterID;
    float PosX, PosY, PosZ; // Poziția inițială în lume

    template <typename Stream>
    void Serialize(Stream& stream) {
        stream& PlayerID;
        stream& CharacterID;
        stream& PosX;
        stream& PosY;
        stream& PosZ;
    }
};

struct Player {
    uint32_t PlayerID;
    uint32_t CharacterID;
    float PosX, PosY, PosZ;
    int Socket; // ID-ul conexiunii jucătorului
};