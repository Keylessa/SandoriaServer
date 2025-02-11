#pragma once

#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;


class Server {
public:
    Server(short port);
    void run();
    void handleClient(tcp::socket socket);

private:
    // Mapping-ul pentru comenzile clientului
    std::unordered_map<std::string, std::function<void(std::istringstream&, tcp::socket&)>> commandHandlers;

    // Funcțiile handle pentru fiecare comandă:
    void handleLogin(std::istringstream& input, tcp::socket& socket);
    void handleCreateCharacter(std::istringstream& input, tcp::socket& socket);
    void handleGetCharacters(std::istringstream& input, tcp::socket& socket);
    void handleCheckName(std::istringstream& input, tcp::socket& socket);

    // Alte variabile
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;

;
};

#endif // SERVER_H