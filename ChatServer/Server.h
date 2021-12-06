#pragma once
#include <vector>
#include <thread>
#include <string>

const int DEFAULT_BUFLEN = 1024;

class Application;
class Server
{
public:
    Server(Application* app);
    ~Server();
    auto run() -> void;
    auto setContinueFlag(bool flag) -> void;
    auto setBufferSize(int index, size_t size)const -> void;
    auto setCashMessage(const std::string& msg, int thread_num )const -> void;
    auto getCashMessage(int thread_num) const -> const std::string&;

private:
    Application* _app;
    bool _continue_flag{true};
};

