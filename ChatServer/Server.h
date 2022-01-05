#pragma once
#include <vector>
#include <thread>
#include <string>
#include <memory>

const int DEFAULT_BUFLEN = 1024;

class Application;
class Server
{
public:
    Server(Application* app);
    ~Server();
    auto run() -> void;
    auto setContinueFlag(bool flag) -> void;
    auto setBufferSize(int index, size_t size) -> void;
    auto getMessageSizeRef(int thread_num) -> size_t& { return _exchange_message_size[thread_num]; }
    auto setMsgFromClientSize(size_t size, int thread_num) -> void { _msg_from_client_size[thread_num] = size; }
    auto getMsgFromClientSize(int thread_num) -> size_t { return _msg_from_client_size[thread_num]; }

    auto getCashMessagePtr(int thread_num) const -> char* { return _cash_message[thread_num].get(); }
    auto getCashMessageSizeRef(int thread_num) -> size_t& { return _cash_message_size[thread_num]; }
    auto resizeCashMessageBuffer(int thread, size_t new_size) -> void;

private:
    Application* _app;
    bool _continue_flag{true};

    std::vector<std::thread> _clients;
    std::vector<std::shared_ptr<char[]>> _cash_message{};  //////////////////////////////////////////////////////////
    std::vector<size_t> _cash_message_size{};              ////////////////////////////////////////////////////
    std::vector<size_t> _cash_message_buffer_size{};       //////////////////////////////////////////////////
    std::vector<size_t> _exchange_buffer_size{};
    std::vector<bool> _need_buffer_resize{};
    std::vector<std::shared_ptr<char[]>> _exchange_message;  /////////////////////////////////////////////////////////
    std::vector<size_t> _exchange_message_size;              /////////////////////////////////////////////////////
    std::vector<size_t> _msg_from_client_size;               /////////////////////////////////////////////////////////////
    std::vector<bool> _in_message_ready{};
    std::vector<bool> _out_message_ready{};

    int thread_count{0};
    volatile bool continue_flag{true};

#ifdef _WIN32
    auto server_thread(int thread_number) -> void;
#elif defined __linux__
    auto client_loop(int thread_number, int connection) -> void;
    auto server_thread() -> int;
#endif
    auto main_loop(Application* app) -> void;
};
