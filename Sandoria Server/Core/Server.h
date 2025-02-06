#pragma once

#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    Server(short port);
    void run();
private:
    void handleClient(tcp::socket socket);
    boost::asio::io_context io_context;
    tcp::acceptor acceptor;
};

#endif // SERVER_H