//Server.cpp
#include "Server.h"
#include "../Auth/AuthManager.h"
#include "../Character/CharacterDatabase.h"
#include "../WorldManager/EnterWorldPacket.h" // Dacă ai o structură pentru pachetul de intrare în lume
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

Server::Server(short port)
    : acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
    // Inițializarea mapping-ului de comenzi
    commandHandlers["LOGIN"] = std::bind(&Server::handleLogin, this, std::placeholders::_1, std::placeholders::_2);
    commandHandlers["CREATE_CHARACTER"] = std::bind(&Server::handleCreateCharacter, this, std::placeholders::_1, std::placeholders::_2);
    commandHandlers["GET_CHARACTERS"] = std::bind(&Server::handleGetCharacters, this, std::placeholders::_1, std::placeholders::_2);
    commandHandlers["CHECK_NAME"] = std::bind(&Server::handleCheckName, this, std::placeholders::_1, std::placeholders::_2);
  
}

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
    try {
        char data[1024] = { 0 };
        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(data), error);
        if (error == boost::asio::error::eof) return;
        if (error) throw boost::system::system_error(error);

        std::string received_data(data, length);
        std::cout << "Received Data: " << received_data << std::endl;

        std::istringstream input(received_data);
        std::string command;
        std::getline(input, command, ':');

        auto it = commandHandlers.find(command);
        if (it != commandHandlers.end()) {
            it->second(input, socket);
        }
        else {
            std::string response = "UNKNOWN_COMMAND\n";
            boost::asio::write(socket, boost::asio::buffer(response));
        }
    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

// --- Implementarea funcției handleLogin ---
void Server::handleLogin(std::istringstream& input, tcp::socket& socket) {
    std::string username, password;
    std::getline(input, username, ':');
    std::getline(input, password, ':');

    int accountID = AuthManager::getAccountID(username, password);
    std::string response;
    if (accountID > 0) {
        response = "SUCCESS:" + std::to_string(accountID) + "\n";
    }
    else {
        response = "FAILED\n";
    }
    boost::asio::write(socket, boost::asio::buffer(response));
}

// --- Implementarea funcției handleCreateCharacter ---
void Server::handleCreateCharacter(std::istringstream& input, tcp::socket& socket) {
    std::string accountID, charName, faction, race, charClass, gender;

    std::getline(input, accountID, ':');
    int accountID_int;
    try {
        accountID_int = std::stoi(accountID);
    }
    catch (const std::exception&) {
        boost::asio::write(socket, boost::asio::buffer("INVALID_ACCOUNT_ID\n"));
        return;
    }

    std::getline(input, charName, ':');
    std::getline(input, faction, ':');
    int faction_int = std::stoi(faction);
    std::getline(input, race, ':');
    int race_int = std::stoi(race);
    std::getline(input, charClass, ':');
    int charClass_int = std::stoi(charClass);
    std::getline(input, gender, ':');
    int gender_int = std::stoi(gender);

    std::cout << "Creare personaj pentru AccountID: " << accountID_int << ", Name: " << charName << std::endl;

    CharacterDatabase charDB;
    std::string response;
    if (charDB.createCharacter(accountID_int, charName, faction_int, race_int, charClass_int, gender_int)) {
        response = "CHARACTER_CREATED\n";
    }
    else {
        response = "FAILED_TO_CREATE_CHARACTER\n";
    }
    boost::asio::write(socket, boost::asio::buffer(response));
}

// --- Implementarea funcției handleGetCharacters ---
void Server::handleGetCharacters(std::istringstream& input, tcp::socket& socket) {
    std::string accountID;
    std::getline(input, accountID, ':');

    CharacterDatabase charDB;
    std::vector<CharacterDataList> characters = charDB.getCharacters(accountID);
    std::string response;
    if (characters.empty()) {
        response = "NO_CHARACTERS\n";
    }
    else {
        response = "CHAR_LIST:";
        for (const auto& character : characters) {
            // Presupunem că structura CharacterData are cel puțin membrii Name, Class și Race
            response += character.Name + "," + character.Level + "," + std::to_string(character.Race) + ";";
        }
        response += "\n";
    }
    boost::asio::write(socket, boost::asio::buffer(response));
}

// --- Implementarea funcției handleCheckName ---
void Server::handleCheckName(std::istringstream& input, tcp::socket& socket) {
    std::string charName;
    std::getline(input, charName, ':');

    CharacterDatabase charDB;
    bool nameExists = charDB.doesCharacterExist(charName);
    std::string response = nameExists ? "NAME_TAKEN\n" : "NAME_AVAILABLE\n";
    boost::asio::write(socket, boost::asio::buffer(response));
}


/////////////////////////////////////////////////////////////
// --- Implementarea funcției handleEnterWorld ---
////////////////////////////////////////////////////////////


/*
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
        std::string command, accountID, charName, faction, race, charClass, gender;

        std::getline(input, command, ':');

        std::string response = "FAILED\n";

        if (command == "LOGIN")
        {
            std::string username, password;
            std::getline(input, username, ':');
            std::getline(input, password, ':');

            int accountID = AuthManager::getAccountID(username, password);
            if (accountID > 0)
            {
                response = "SUCCESS:" + std::to_string(accountID) + "\n";
            }
            else
            {
                response = "FAILED\n";
            }

            boost::asio::write(socket, boost::asio::buffer(response));
        }

        else if (command == "CREATE_CHARACTER")
        {
            std::getline(input, accountID, ':');
            int accountID_int = std::stoi(accountID);
            std::getline(input, charName, ':');
            std::getline(input, faction, ':');
            int faction_int = std::stoi(faction);
            std::getline(input, race, ':');
            int race_int = std::stoi(race);
            std::getline(input, charClass, ':');
            int charClass_int = std::stoi(charClass);
            std::getline(input, gender, ':');
            int gender_int = std::stoi(gender);

            std::cout << "Creare personaj pentru AccountID: " << accountID_int << ", Name: " << charName << std::endl;

            CharacterDatabase charDB;
            if (charDB.createCharacter(accountID_int, charName, faction_int, race_int, charClass_int, gender_int)) {
                response = "CHARACTER_CREATED\n";
            }
            else {
                response = "FAILED_TO_CREATE_CHARACTER\n";
            }

            boost::asio::write(socket, boost::asio::buffer(response));
        }

        else if (command == "GET_CHARACTERS")
        {
            std::getline(input, accountID, ':');

            CharacterDatabase charDB;
            std::vector<CharacterData> characters = charDB.getCharacters(accountID);

            if (characters.empty()) {
                response = "NO_CHARACTERS\n";
            }
            else {
                response = "CHAR_LIST:";
                for (const auto& character : characters) {
                    response += character.Name + "," + character.Class + "," + character.Race + ";";
                }
                response += "\n";
            }

            boost::asio::write(socket, boost::asio::buffer(response));
        }

        else if (command == "CHECK_NAME")
        {
            std::getline(input, charName, ':');

            CharacterDatabase charDB;
            bool nameExists = charDB.doesCharacterExist(charName);

            response = nameExists ? "NAME_TAKEN\n" : "NAME_AVAILABLE\n";
            boost::asio::write(socket, boost::asio::buffer(response));
        }

        else if (command == "ENTER_WORLD")
        {
            std::getline(input, charName, ':');
            std::string clientIP = socket.remote_endpoint().address().to_string();

            std::cout << "Entering world with character: " << charName << " from " << clientIP << std::endl;

            // Salvăm jucătorul în lista celor conectați
            activePlayers[clientIP] = charName;

            // Construim mesajul cu toți jucătorii existenți
            std::string playerList = "PLAYER_LIST:";
            for (const auto& [ip, name] : activePlayers)
            {
                playerList += name + ",";
            }
            if (!activePlayers.empty())
            {
                playerList.pop_back(); // Elimină ultima virgulă
            }
            playerList += "\n";

            // Trimitem lista la toți clienții
            for (const auto& [ip, name] : activePlayers)
            {
                boost::asio::write(socket, boost::asio::buffer(playerList));
            }

            // Trimitem mesajul de succes
            response = "ENTER_SUCCESS\n";
            boost::asio::write(socket, boost::asio::buffer(response));

            // Informăm toți clienții că un nou jucător a intrat
            std::string newPlayerMsg = "NEW_PLAYER:" + charName + "\n";
            for (const auto& [ip, name] : activePlayers)
            {
                boost::asio::write(socket, boost::asio::buffer(newPlayerMsg));
            }
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

*/