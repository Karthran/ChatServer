#pragma once
#include <string>
#include <fstream>
#include <thread>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <map>

class File;

class Logger
{
public:
    Logger(const std::string& file_name);
    ~Logger();
    auto fileRef() -> std::fstream& { return _filestream; }
    auto saveLog(const std::string& user_id, const std::string& message) -> void;
    auto loadLog(std::string& user_id, std::string& message, int message_index) -> void;
    auto stopLogger() -> void { is_continue = false; }

private:
    std::fstream _filestream;
    std::shared_mutex _file_mutex{};
    std::mutex _mutex{};

    volatile bool is_continue{true};

    auto write_log(const std::string& user_id, const std::string& message) -> void;
    auto read_log(std::string& user_id, std::string& message, int message_index) -> void;
};
