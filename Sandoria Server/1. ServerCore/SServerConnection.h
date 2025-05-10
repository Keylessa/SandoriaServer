///////////////////////////////////
// SServerConnection.h
//////////////////////////////////
#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>

class SServerConnection
{
public:
    SServerConnection();
    ~SServerConnection();

    bool Initialize();       // Inițializare server și conexiuni
    void Update();           // Procesare pachete de la clienți
    void StartListening();   // Pornește serverul de ascultare

private:
    void AcceptClient();     // Acceptă un client și începe să-l proceseze
    void HandleClient(std::shared_ptr<boost::asio::ip::tcp::socket> clientSocket);

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> client_sockets;
    std::vector<std::thread> client_threads;
    std::mutex clients_mutex;
};
