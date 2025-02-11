#pragma once

#include <cstdint>

struct EnterWorldPacket {
    uint32_t PlayerID;
    uint32_t CharacterID;

    template <typename Stream>
    void Serialize(Stream& stream) {
        stream& PlayerID;
        stream& CharacterID;
    }
};
