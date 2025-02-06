
#include "Server.h"
#include "../Auth/AuthManager.h"
#include <iostream>
#include <thread>

Server::Server(short port) : acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

void Server::run() {
    std::cout << "Server listening on port " << acceptor.local_endpoint().port() << "..." << std::endl;
    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::thread(&Server::handleClient, this, std::move(socket)).detach();
    }
}

void Server::handleClient(tcp::socket socket) {
    try {
        char data[1024] = { 0 };
        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(data), error);

        if (error == boost::asio::error::eof) return;
        else if (error) throw boost::system::system_error(error);

        std::string received_data(data, length);
        std::cout << "Received Data: " << received_data << std::endl;

        std::istringstream input(received_data);
        std::string command, username, password;

        std::getline(input, command, ':');
        std::getline(input, username, ':');
        std::getline(input, password, ':');

        std::string response = "FAILED\n";
        if (command == "LOGIN") {
            response = AuthManager::authenticateUser(username, password) ? "SUCCESS\n" : "FAILED\n";
        }

        boost::asio::write(socket, boost::asio::buffer(response), error);
    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}