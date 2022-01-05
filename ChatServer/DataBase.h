#pragma once

struct MYSQL;

class DataBase
{
public:
    DataBase(const char* server, const char* user, const char* password, const char* database, unsigned int port)
        : _server(server),      //
          _user(user),          //
          _password(password),  //
          _database(database),  //
          _port(port)           //
    {
    }
    ~DataBase();

    auto init() -> void;
    auto connect() -> void;
    auto query(const char* sql_query) -> int;
    auto getQueryResult(std::string& result, int& row_num, int& column_num) -> void;

private:
    MYSQL* _mysql{nullptr};
    const char* _server{nullptr};
    const char* _user{nullptr};
    const char* _password{nullptr};
    const char* _database{nullptr};
    unsigned int _port{0};
};
