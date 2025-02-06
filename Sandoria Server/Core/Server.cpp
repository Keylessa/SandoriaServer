
#include "Server.h"
#include "../Auth/AuthManager.h"
#include "../Character/CharacterDatabase.h"
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
        std::string command, accountName, charName, race, charClass, gender;

        std::getline(input, command, ':');

        std::string response = "FAILED\n";
        if (command == "LOGIN") {
            std::string username, password;
            std::getline(input, username, ':');
            std::getline(input, password, ':');

            response = AuthManager::authenticateUser(username, password) ? "SUCCESS\n" : "FAILED\n";
        }

        //std::string response = "FAILED\n";
        if (command == "CREATE_CHARACTER") {
            std::getline(input, accountName, ':');
            std::getline(input, charName, ':');
            std::getline(input, race, ':');
            std::getline(input, charClass, ':');
            std::getline(input, gender, ':');

            std::cout << "Creare personaj: " << accountName << ", " << charName << ", " << race << ", " << charClass << ", " << gender << std::endl;

            CharacterDatabase charDB;
            if (charDB.createCharacter(accountName, charName, race, charClass, gender)) {
                std::cout << "Personaj creat cu succes!" << std::endl;
                response = "CHARACTER_CREATED\n";
            }
            else {
                std::cout << "Eroare la crearea personajului!" << std::endl;
                response = "FAILED_TO_CREATE_CHARACTER\n";
            }
            
            std::cout << "Răspuns final către client: " << response << std::endl;  // Log pentru a verifica ce trimite serverul

        }
        std::cout << "Trimitere răspuns către client: " << response << std::endl;
        boost::asio::write(socket, boost::asio::buffer(response), error);
if (error) {
    std::cerr << "Eroare la trimiterea răspunsului: " << error.message() << std::endl;  // Log pentru erori
} else {
    std::cout << "Răspuns trimis cu succes: " << response << std::endl;  // Log pentru succes
}    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}