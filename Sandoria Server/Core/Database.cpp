#include "Database.h"

Database::Database(const std::string& dbName) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        conn = std::unique_ptr<sql::Connection>(driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASS));
        conn->setSchema(dbName);
    }
    catch (sql::SQLException& e) {
        std::cerr << "MySQL Connection Error: " << e.what() << std::endl;
    }
}

Database::~Database() {}

std::unique_ptr<sql::PreparedStatement> Database::prepareStatement(const std::string& query) {
    return std::unique_ptr<sql::PreparedStatement>(conn->prepareStatement(query));
}