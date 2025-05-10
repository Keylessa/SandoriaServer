#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "STCPConnection.h"

using boost::asio::ip::tcp;

class SNetworkHandler
{
public:
    SNetworkHandler();
    ~SNetworkHandler();

    bool Initialize(int Port);
    void Update();

private:
    void AcceptConnection();

    boost::asio::io_context IoContext;
    tcp::acceptor Acceptor;
    std::vector<std::shared_ptr<STCPConnection>> Clients;
};
