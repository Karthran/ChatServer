#include "Logger.h"

Logger::Logger(const std::string& file_name)
{
    _filestream.open("log.txt", std::ios::out | std::ios::in | std::ios::app);
}

Logger::~Logger()
{
    _filestream.close();
}

auto Logger::write_log(const std::string& user_id, const std::string& message) -> void
{
    auto msg_size{message.size()};
    _file_mutex.lock();
    _filestream << user_id << " ";
    _filestream << msg_size << " ";
    std::string msg(message);
    msg += '\n';
    _filestream << msg;
    _file_mutex.unlock();
}

auto Logger::read_log(std::string& user_id, std::string& message, int message_index) -> void
{
    auto index{0};
    user_id.clear();
    message.clear();
    auto msg_size{0};
    while (!_filestream.eof())
    {
        _file_mutex.lock_shared();
        _filestream >> user_id;
        _filestream >> msg_size;
        std::shared_ptr<char[]> str = std::shared_ptr<char[]>(new char[msg_size + 2]);  //+1 for '\0' , +1 for separator
        _filestream.getline(str.get(), msg_size + 2, '\n');                             //
        _file_mutex.unlock_shared();
        message = (str.get() + 1);  // skip the separator
        ++index;
        if (index == message_index) break;
    }
    if (_filestream.eof()) 
    {
        user_id.clear();
        message.clear();
    }
}

auto Logger::saveLog(const std::string& user_id, const std::string& message) -> void
{
    auto t = std::thread(&Logger::write_log, this, std::ref(user_id), std::ref(message));
    t.join();
}

auto Logger::loadLog(std::string& user_id, std::string& message, int message_index) -> void
{
    auto t = std::thread(&Logger::read_log, this, std::ref(user_id), std::ref(message), message_index);
    t.join();
}
