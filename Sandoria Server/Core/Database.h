// Database.h - Gestionarea conexiunii cu MySQL
#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include "Config.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <iostream>

class Database {
public:
    Database(const std::string& dbName);
    ~Database();
    std::unique_ptr<sql::PreparedStatement> prepareStatement(const std::string& query);
private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;
};

#endif // DATABASE_H