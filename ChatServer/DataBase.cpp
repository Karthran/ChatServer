#include <iostream>
#include <mysql.h>

#include "DataBase.h"

DataBase::~DataBase()
{  // Закрываем соединение с сервером базы данных
    mysql_close(_mysql);
}

auto DataBase::init() -> void
{  // Получаем дескриптор соединения
    _mysql = mysql_init(NULL);
    if (_mysql == nullptr)
    {
        // Если дескриптор не получен — выводим сообщение об ошибке
        std::cout << "Error: can't create MySQL-descriptor" << std::endl;
    }
}

auto DataBase::connect() -> void
{  // Подключаемся к серверу
    if (!mysql_real_connect(_mysql, _server, _user, _password, _database, _port, NULL, 0))
    {
        // Если нет возможности установить соединение с БД выводим сообщение об ошибке
        std::cout << "Error: can't connect to database " << mysql_error(_mysql) << std::endl;
    }
    else
    {
        // Если соединение успешно установлено выводим фразу — "Success!"
        std::cout << "Success!" << std::endl;
    }
    mysql_set_character_set(_mysql, "utf8");
    std::cout << "connection characterset: " << mysql_character_set_name(_mysql) << std::endl;
}

auto DataBase::query(const char* sql_query) -> int
{
    return mysql_query(_mysql, sql_query);
}

auto DataBase::getQueryResult(std::string& result, int& row_num, int& column_num) -> void
{
    MYSQL_RES* res{nullptr};
    MYSQL_ROW row{};

    row_num = 0;
    if (res = mysql_store_result(_mysql))
    {
        column_num = mysql_num_fields(res);
        auto size_res{sizeof(res)};

        while (row = mysql_fetch_row(res))
        {
            ++row_num;
            for (auto i{0}; i < column_num; ++i)
            {
                if (row[i])
                    result += row[i];
                else
                    result += "NULL";
                result.push_back('0');
                result[result.size()] = '\0';

                std::cout << row[i] << "  ";
            }
            std::cout << std::endl;
        }
    }
    else
        std::cout << "Ошибка MySql номер " << mysql_error(_mysql);
}
