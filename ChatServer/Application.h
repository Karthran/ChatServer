#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include "DataBase.h"
#include "Server.h"

class Application
{
public:
    Application();

    void run();
    auto reaction(char* message, int thread_num) -> void;

private:
    std::unique_ptr<Server> _server{};
    std::unique_ptr<DataBase> _data_base{};

    int _current_user_number{0};
    int _current_chat_number{0};

    std::string _self_path{};

    std::vector<int> _signed_user{};
    std::map<int, std::string> _connected_user_id{};



    /* string_arr{0] is Menu Name , printed with underline and without number*/
    auto menu(std::string* string_arr, int size) const -> int;

    auto onCheckSize(char* message, int thread_num) const -> void;
    auto onCheckEmail(char* message, int thread_num) -> void;
    auto onCheckLogin(char* message, int thread_num) -> void;
    auto onRegistration(char* message, int thread_num) -> void;
    auto onSignIn(char* message, int thread_num) -> void;
    auto onCommonChatGetMessages(char* message, int thread_num) -> void;
    auto onCommonChatAddMessage(char* message, int thread_num) -> void;
    auto onCommonChatCheckMessage(char* message, int thread_num) -> void;
    auto onPrivateChatCheckMessage(char* message, int thread_num) -> void;
    auto onCommonChatEditMessage(char* message, int thread_num) -> void;
    auto onCommonChatDeleteMessage(char* message, int thread_num) -> void;
    auto onNewMessagesInCommonChat(char* message, int thread_num) -> void;
    auto onNewMessagesInPrivateChat(char* message, int thread_num) -> void;
    auto onViewUsersIDNameSurname(char* message, int thread_num) -> void;
    auto onViewUsersWithNewMessages(char* message, int thread_num) -> void;
    auto onViewUsersWithPrivateChat(char* message, int thread_num) -> void;
    auto onGetPrivateChatID(char* message, int thread_num) -> void;
    auto onPrivateChatAddMessage(char* message, int thread_num) -> void;
    auto onPrivateChatGetMessages(char* message, int thread_num) -> void;
    auto onPrivateChatEditMessages(char* message, int thread_num) -> void;
    auto onPrivateChatDeleteMessages(char* message, int thread_num) -> void;

    auto onStop(char* message, int thread_num) -> void;
    auto onError(char* message, int thread_num) const -> void;

    auto addToBuffer(char* buffer, size_t& cur_msg_len, int value) const -> void;
    auto addToBuffer(char* buffer, size_t& cur_msg_len, const char* string, size_t str_len) const -> void;

    auto getFromBuffer(const char* buffer, size_t shift, int& value) const -> void;
    auto getFromBuffer(const char* buffer, size_t shift, char* string, size_t str_len) const -> void;

    auto checkEmail(char* email, size_t email_size, int thread_num) -> void;
    auto checkLogin(char* login, size_t login_size, int thread_num) -> void;
    auto registration(char* regdata, size_t regdata_size, int thread_num) -> void;
    auto signin(char* signin_data, size_t signin_data_size, int thread_num) -> void;
    auto commonChatGetMessages(char* data, size_t data_size, int thread_num) -> void;
    auto commonChatAddMessage(char* message, size_t message_size, int thread_num) -> void;
    auto commonChatCheckMessage(char* message, size_t message_size, int thread_num) -> void;
    auto privateChatCheckMessage(char* message, size_t message_size, int thread_num) -> void;
    auto commonChatEditMessage(char* message, size_t message_size, int thread_num) -> void;
    auto commonChatDeleteMessage(char* message, size_t message_size, int thread_num) -> void;
    auto newMessagesInCommonChat(char* message, size_t message_size, int thread_num) -> void;
    auto newMessagesInPrivateChat(char* message, size_t message_size, int thread_num) -> void;
    auto viewUsersIDNameSurname(char* message, size_t message_size, int thread_num) -> void;
    auto viewUsersWithNewMessages(char* message, size_t message_size, int thread_num) -> void;
    auto viewUsersWithPrivateChat(char* message, size_t message_size, int thread_num) -> void;
    auto getPrivateChatID(char* message, size_t message_size, int thread_num) -> void;
    auto privateChatAddMessage(char* message, size_t message_size, int thread_num) -> void;
    auto privateChatGetMessages(char* message, size_t message_size, int thread_num) -> void;
    auto privateChatEditMessages(char* message, size_t message_size, int thread_num) -> void;
    auto privateChatDeleteMessages(char* message, size_t message_size, int thread_num) -> void;

    auto createDataBases() -> void;
};
