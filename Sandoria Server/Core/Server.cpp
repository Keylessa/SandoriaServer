
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

void Server::handleClient(tcp::socket socket)
{
    try
    {
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

        if (command == "LOGIN")
        {
            std::string username, password;
            std::getline(input, username, ':');
            std::getline(input, password, ':');

            bool isAuthenticated = AuthManager::authenticateUser(username, password);
            response = isAuthenticated ? "SUCCESS\n" : "FAILED\n";

            boost::asio::write(socket, boost::asio::buffer(response));

            // Închidem conexiunea DOAR după ce am trimis mesajul de eșec
            if (!isAuthenticated)
            {
                socket.close();
                return;
            }
        }
        else if (command == "CREATE_CHARACTER")
        {
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
        }
        else if (command == "GET_CHARACTERS")
        {
            std::getline(input, accountName, ':');

            CharacterDatabase charDB;
            std::vector<CharacterData> characters = charDB.getCharacters(accountName);

            if (characters.empty()) {
                response = "NO_CHARACTERS\n";
            }
            else {
                response = "CHAR_LIST:";
                for (const auto& character : characters) {
                    response += character.name + "," + character.charClass + "," + character.race + ";";
                }
                response += "\n";
            }
        }
        else if (command == "CHECK_NAME")
        {
            std::getline(input, charName, ':');

            CharacterDatabase charDB;
            bool nameExists = charDB.doesCharacterExist(charName);

            response = nameExists ? "NAME_TAKEN\n" : "NAME_AVAILABLE\n";
        }

        std::cout << "Trimitere răspuns către client: " << response << std::endl;
        boost::asio::write(socket, boost::asio::buffer(response), error);
        if (error) {
            std::cerr << "Eroare la trimiterea răspunsului: " << error.message() << std::endl;
        }
        else {
            std::cout << "Răspuns trimis cu succes: " << response << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}
