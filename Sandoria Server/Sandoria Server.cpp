// Sandoria Server.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "./Core/Server.h"
#include <iostream>

int main() {
    try {
        Server server(12345);
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
    return 0;
}