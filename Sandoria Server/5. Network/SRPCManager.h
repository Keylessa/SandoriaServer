#pragma once

#include <iostream>
#include <string.h>

class SRPCManager
{
public:
    SRPCManager();
    ~SRPCManager();

    void CallRPC(int rpcID, const std::string& parameters); // Apelare RPC pe server

private:
    // Poți adăuga un map pentru a lega ID-urile RPC de funcțiile lor
};
