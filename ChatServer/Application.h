#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

class Server;


class Chat;
class User;
class PasswordHash;
class NewMessages;

class Application
{
public:
    Application();

    void run();
    auto reaction(const std::string& message, std::string& out_message, int thread_num) -> void;

private:
    std::vector<std::shared_ptr<User>> _user_array;
    int _current_user_number{0};

    std::shared_ptr<Chat> _common_chat;

    std::map<long long, std::shared_ptr<Chat>> _private_chat_array;
    int _current_chat_number{0};

    std::unordered_map<std::string, std::shared_ptr<PasswordHash>> _password_hash;

    std::vector<std::shared_ptr<NewMessages>> _new_messages_array;

    std::string _self_path{};

    Server* _server{ nullptr };

    auto createAccount() -> int;
    auto createAccount_inputName(std::string& name) const -> void;
    auto createAccount_inputLogin(std::string& login) const -> void;
    auto createAccount_inputPassword(std::string& password) const -> void;

    auto signIn() -> int;
    auto signIn_inputLogin(std::string& user_login) const -> int;
    auto signIn_inputPassword(std::string& user_password) const -> void;

    auto selectCommonOrPrivate(const std::shared_ptr<User>& user) -> int;

    auto commonChat(const std::shared_ptr<User>& user) const -> int;
    auto commonChat_addMessage(const std::shared_ptr<User>& user) const -> void;
    auto commonChat_editMessage(const std::shared_ptr<User>& user) const -> void;
    auto commonChat_deleteMessage(const std::shared_ptr<User>& user) const -> void;

    auto privateMenu(const std::shared_ptr<User>& user) -> int;
    auto privateMenu_viewUsersNames() const -> void;
    auto privateMenu_selectByName(const std::shared_ptr<User>& user) const -> int;
    auto privateMenu_selectByID(const std::shared_ptr<User>& user) -> void;
    auto printNewMessagesUsers(const std::shared_ptr<User>& user) -> void;

    auto privateChat(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user) -> int;

    auto privateChat_addMessage(
        const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user, std::shared_ptr<Chat>& chat) -> void;

    auto privateChat_editMessage(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user,
        const std::shared_ptr<Chat>& chat) const -> void;

    auto privateChat_deleteMessage(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user,
        const std::shared_ptr<Chat>& chat) const -> void;

    /*Finds chat in array, return empty shared_ptr if chat don't exist */
    auto getPrivateChat(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user) const
        -> const std::shared_ptr<Chat>;

    /*Searches for matching line*/
    auto checkingForStringExistence(const std::string& string, const std::string& (User::*get)() const) const -> int;

    /* string_arr{0] is Menu Name , printed with underline and without number*/
    auto menu(std::string* string_arr, int size) const -> int;

    auto save() -> void;
    auto saveUserArray() const -> bool;
    auto savePasswordHash() -> void;
    auto saveChats() const -> void;
    auto saveNewMessages() -> void;

    auto load() -> void;
    auto loadUserArray() -> bool;
    auto loadPasswordHash() -> void;
    auto loadChats() -> void;
    auto loadNewMessages() -> void;

    auto onCheckSize(const std::string& in_message, std::string& out_message, int thread_num) const -> void;
    auto onCheckName(const std::string& in_message, std::string& out_message, int thread_num) -> void;
    auto onStop(const std::string& in_message, std::string& out_message, int thread_num) -> void;
    auto onError(std::string& out_message) const -> void;

    /////////////////////////DEBUG//////////////////////////////
    auto checkName(const std::string& name) -> const std::string;
    /////////////////////////DEBUG//////////////////////////////

};
