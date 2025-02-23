#include "SNetworkHandler.h"
#include <iostream>

SNetworkHandler::SNetworkHandler()
    : Acceptor(IoContext)
{
}

SNetworkHandler::~SNetworkHandler()
{
}

bool SNetworkHandler::Initialize(int Port)
{
    try
    {
        tcp::endpoint Endpoint(tcp::v4(), Port);
        Acceptor.open(Endpoint.protocol());
        Acceptor.bind(Endpoint);
        Acceptor.listen();

        std::cout << "Server listening on port " << Port << "..." << std::endl;
        AcceptConnection();
        return true;
    }
    catch (std::exception& e)
    {
        std::cerr << "Server failed to start: " << e.what() << std::endl;
        return false;
    }
}

void SNetworkHandler::AcceptConnection()
{
    auto Client = std::make_shared<STCPConnection>(IoContext);
    Acceptor.async_accept(Client->GetSocket(), [this, Client](boost::system::error_code ec) {
        if (!ec)
        {
            std::cout << "New client connected!" << std::endl;
            Clients.push_back(Client);
            Client->Start();
        }
        AcceptConnection();  // Continuă să accepte conexiuni noi
        });
}

void SNetworkHandler::Update()
{
    IoContext.run();
}
