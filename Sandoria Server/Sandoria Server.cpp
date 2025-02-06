// Sandoria Server.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#define MYSQL_HOST "tcp://127.0.0.1:3307"
#define MYSQL_USER "root"
#define MYSQL_PASS "espada076@"
#define MYSQL_DB "account"

bool authenticateUser(const std::string& username, const std::string& password) 
{
    try 
    {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS));
        conn->setSchema(MYSQL_DB);

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT * FROM users WHERE username = ? AND password = ?"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        return res->next(); // Dacă există un rezultat, autentificarea e validă
    }

    catch (sql::SQLException& e) {
        std::cerr << "MySQL error: " << e.what() << std::endl;
        return false;
    }
}
void handle_client(tcp::socket socket)
{
    try
    {
        char data[1024] = { 0 };
        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(data), error);

        if (error == boost::asio::error::eof)
        {
            return; // Clientul s-a deconectat
        }
        else if (error)
        {
            throw boost::system::system_error(error);
        }

        std::string received_data(data, length);
        std::cout << "Received Data: " << received_data << std::endl;

        std::istringstream input(received_data);
        std::string command, username, password;

        std::getline(input, command, ':');
        std::getline(input, username, ':');
        std::getline(input, password, ':');

        std::string response = "FAILED\n";
        if (command == "LOGIN")
        {
            bool authSuccess = authenticateUser(username, password);
            response = authSuccess ? "SUCCESS\n" : "FAILED\n";
        }

        boost::asio::write(socket, boost::asio::buffer(response), error);
    }
    catch (std::exception& e)
    {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Server listening on port 12345..." << std::endl;

        while (true)
        {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(handle_client, std::move(socket)).detach();
        }
    }

    catch (std::exception& e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
    return 0;
}
