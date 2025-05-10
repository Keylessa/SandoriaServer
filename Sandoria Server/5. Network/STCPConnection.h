#pragma once
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class STCPConnection : public std::enable_shared_from_this<STCPConnection>
{
public:
    explicit STCPConnection(boost::asio::io_context& IoContext);
    tcp::socket& GetSocket();
    void Start();
    void ReadData();

private:
    tcp::socket Socket;
    char Buffer[1024];
};
