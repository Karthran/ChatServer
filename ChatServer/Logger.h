#pragma once
#include <string>
#include <fstream>
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

    auto getOutMessageReady(int server_thread_num) -> bool { return _out_msg_ready[server_thread_num]; }
    auto setOutMessageReady(int server_thread_num, bool flag) -> void { _out_msg_ready[server_thread_num] = flag; }
    auto getInMessageReady(int server_thread_num) -> bool { return _in_msg_ready[server_thread_num]; }
    auto setInMessageReady(int server_thread_num, bool flag) -> void { _in_msg_ready[server_thread_num] = flag; }
    auto getMessage(int server_thread_num) -> const std::string& { return _message[server_thread_num]; }
    auto setMessage(int server_thread_num, const std::string msg) -> void
    {
        _message[server_thread_num] = msg;
        _out_msg_ready[server_thread_num] = true;
    }
    auto stopLogger() -> void { is_continue = false; }

private:
    std::fstream _filestream;
    std::map<int, std::thread> _threads;
    std::map<int, std::string> _message;
    std::map<int, bool> _out_msg_ready;
    std::map<int, bool> _in_msg_ready;
    int _thread_count{0};
    std::shared_mutex _file_mutex{};
    std::mutex _mutex{};

    volatile bool is_continue{true};

    auto write_log(const std::string& user_id, const std::string& message) -> void;
    auto read_log(std::string& user_id, std::string& message, int message_index) -> void;
};
