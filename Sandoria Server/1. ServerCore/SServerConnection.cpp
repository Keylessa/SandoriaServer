#include "SServerConnection.h"
#include "../8. Globals/Globals.h"
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include "../2. AuthServer/SAuthManager.h" // Asigură-te că calea este corectă

SServerConnection::SServerConnection()
    : acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 7777))
{
}

SServerConnection::~SServerConnection()
{
    for (std::thread& t : client_threads)
    {
        if (t.joinable())
            t.join();
    }
}

bool SServerConnection::Initialize()
{
    try {
        StartListening();
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing server: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void SServerConnection::StartListening()
{
    std::cout << "Server listening on port 7777..." << std::endl;
    AcceptClient();
    io_context.run();
}

void SServerConnection::AcceptClient()
{
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error)
        {
            if (!error)
            {
                std::cout << "Client connected!" << std::endl;
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    client_sockets.push_back(socket);
                }
                client_threads.emplace_back(&SServerConnection::HandleClient, this, socket);
            }
            else
            {
                std::cerr << "Error accepting client: " << error.message() << std::endl;
            }
            AcceptClient();
        });
}

void SServerConnection::Update()
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& clientSocket : client_sockets)
    {
        if (!clientSocket || !clientSocket->is_open()) continue;
        char buffer[512] = { 0 };
        boost::system::error_code error;
        size_t length = clientSocket->read_some(boost::asio::buffer(buffer), error);
        if (!error)
        {
            std::string data(buffer, length);
            std::cout << "Received packet: " << data << std::endl;
        }
    }
}

void SServerConnection::HandleClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket)
{
    try {
        while (clientSocket->is_open())
        {
            char buffer[512] = { 0 };
            boost::system::error_code error;
            size_t length = clientSocket->read_some(boost::asio::buffer(buffer), error);

            if (!error)
            {
                std::string data(buffer, length);
                std::cout << "Received packet: " << data << std::endl;

                if (data.find("LOGIN|") == 0)
                {
                    std::istringstream ss(data);
                    std::string command, username, password;
                    std::getline(ss, command, '|');
                    std::getline(ss, username, '|');
                    std::getline(ss, password, '|');

                    SAuthManager authManager(globalDatabaseManager);
                    std::string response = authManager.VerifyLogin(username, password) ? "LOGIN_SUCCESS" : "LOGIN_FAIL";
                    boost::asio::write(*clientSocket, boost::asio::buffer(response));
                }
            }
            else
            {
                std::cerr << "Client disconnected: " << error.message() << std::endl;
                break;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }
}