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
    commandHandlers["ENTER_WORLD"] = std::bind(&Server::handleEnterWorld, this, std::placeholders::_1, std::placeholders::_2);
  
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

void Server::HandleEnterWorldPacket(int clientSocket, EnterWorldPacket packet)
{
    std::lock_guard<std::mutex> lock(mutex); // Protejăm accesul la lista jucătorilor

    // Creăm un nou obiect Player
    Player newPlayer;
    newPlayer.PlayerID = packet.PlayerID;
    newPlayer.CharacterID = packet.CharacterID;
    newPlayer.PosX = packet.PosX;
    newPlayer.PosY = packet.PosY;
    newPlayer.PosZ = packet.PosZ;
    newPlayer.Socket = clientSocket;

    players[packet.PlayerID] = newPlayer; // Adăugăm jucătorul în lista globală

    // Informăm ceilalți jucători despre noul jucător
    for (const auto& [id, player] : players) {
        if (id != packet.PlayerID) {
            SendEnterWorldToClient(player.Socket, newPlayer);
        }
    }

    // Trimitem lista jucătorilor existenți noului jucător
    for (const auto& [id, player] : players) {
        if (id != packet.PlayerID) {
            SendEnterWorldToClient(clientSocket, player);
        }
    }

    std::cout << "Jucator " << packet.PlayerID << " a intrat in lume." << std::endl;
}

void Server::SendEnterWorldToClient(int clientSocket, const Player& player)
{
    EnterWorldPacket packet;
    packet.PlayerID = player.PlayerID;
    packet.CharacterID = player.CharacterID;
    packet.PosX = player.PosX;
    packet.PosY = player.PosY;
    packet.PosZ = player.PosZ;

    // Trimitem pachetul către client
    send(clientSocket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
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

void Server::handleEnterWorld(std::istringstream& input, tcp::socket& socket)
{
    // Se presupune că datele vin în formatul:
    // "ENTER_WORLD:PlayerID,CharacterID,PosX,PosY,PosZ"
    std::string packetData;
    std::getline(input, packetData);  // Obține restul liniei (de exemplu "123,456,10.0,20.0,30.0")

    std::istringstream packetStream(packetData);
    std::string token;
    EnterWorldPacket packet;  // Asigură-te că includeți header-ul corespunzător

    // Parsează fiecare câmp din CSV
    if (std::getline(packetStream, token, ',')) {
        packet.PlayerID = std::stoi(token);
    }
    if (std::getline(packetStream, token, ',')) {
        packet.CharacterID = std::stoi(token);
    }
    if (std::getline(packetStream, token, ',')) {
        packet.PosX = std::stof(token);
    }
    if (std::getline(packetStream, token, ',')) {
        packet.PosY = std::stof(token);
    }
    if (std::getline(packetStream, token, ',')) {
        packet.PosZ = std::stof(token);
    }

    // Pentru a folosi funcția existentă, trebuie să obții un descriptor nativ de la socket
    int nativeSocket = socket.native_handle();
    HandleEnterWorldPacket(nativeSocket, packet);

    // Răspunde clientului cu un mesaj de succes
    std::string response = "ENTER_SUCCESS\n";
    boost::asio::write(socket, boost::asio::buffer(response));
}