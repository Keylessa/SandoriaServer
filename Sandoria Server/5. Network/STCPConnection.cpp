#include "STCPConnection.h"
#include <iostream>

STCPConnection::STCPConnection(boost::asio::io_context& IoContext)
    : Socket(IoContext)
{
}

tcp::socket& STCPConnection::GetSocket()
{
    return Socket;
}

void STCPConnection::Start()
{
    ReadData();
}

void STCPConnection::ReadData()
{
    auto Self(shared_from_this());
    Socket.async_read_some(boost::asio::buffer(Buffer, sizeof(Buffer)),
        [this, Self](boost::system::error_code ec, std::size_t Length)
        {
            if (!ec)
            {
                std::cout << "Received data: " << std::string(Buffer, Length) << std::endl;
                ReadData();  // Continuă să citească date
            }
            else
            {
                std::cerr << "Client disconnected." << std::endl;
            }
        });
}
