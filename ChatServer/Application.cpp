#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include<cstring>
#include <cassert>
#include <iomanip>
#include <exception>
#include <fstream>
#include <sstream>
#include <mutex>

#include "Application.h"
#include "Server.h"
#include "core.h"
#include "Utils.h"
//#include "SHA1.h"

//#include "Chat.h"
//#include "Message.h"
//#include "User.h"
//#include "SHA1.h"
//#include "PasswordHash.h"
//#include "FileUtils.h"
//#include "NewMessages.h"

std::mutex mutex{};

Application::Application()
{
    Utils::getSelfPath(_self_path);
}

auto Application::run() -> void
{
    Utils::printOSVersion();

    std::cout << std::endl << BOLDYELLOW << UNDER_LINE << "Wellcome to Console Chat Dedicated Server!" << RESET << std::endl;

    std::cout << "Type 'end' and press Enter for quit!" << std::endl;

    //std::string server_address{};
    //std::cout << "MySQL server address(press Enter for default 'localhost'): " << std::endl;
    //std::getline(std::cin, server_address);
    //if (!server_address.size()) server_address = "localhost";


    //   load();

    _data_base = std::make_unique<DataBase>("localhost", "root", "rksm", "testdb", 0);
    _data_base->init();
    _data_base->connect();

    createDataBases();

    _server = new Server(this);
    _server->run();

    std::string msg{};
    while (true)
    {
        std::cin >> msg;
        if (msg == "end")
        {
            _server->setContinueFlag(false);
            break;
        }
    }

    //   save();
    delete _server;

    return;
}

// auto Application::createAccount() -> int
//{
//    std::string user_name{};
//    createAccount_inputName(user_name);
//
//    std::string user_login;
//    createAccount_inputLogin(user_login);
//
//    std::string user_password;
//    createAccount_inputPassword(user_password);
//
//    std::cout << BOLDYELLOW << std::endl << "Create account?(Y/N): " << BOLDGREEN;
//    if (!Utils::isOKSelect()) return UNSUCCESSFUL;
//
//    _user_array.push_back(std::make_shared<User>(user_name, user_login, _current_user_number));
//
//    _new_messages_array.push_back(std::make_shared<NewMessages>());
//
//    const std::string salt = getSalt();
//    std::shared_ptr<PasswordHash> password_hash = sha1(user_password, salt);
//    _password_hash[user_login] = password_hash;
//
//    return ++_current_user_number;
//}
//
// auto Application::createAccount_inputName(std::string& user_name) const -> void
//{
//    std::cout << std::endl;
//    std::cout << BOLDYELLOW << UNDER_LINE << "Create account:" << RESET << std::endl;
//    auto isOK{false};
//    while (!isOK)
//    {
//        std::cout << "Name(max " << MAX_INPUT_SIZE << " letters): ";
//        std::cout << BOLDGREEN;
//        Utils::getString(user_name, MAX_INPUT_SIZE);
//        std::cout << RESET;
//        const std::string& (User::*get_name)() const = &User::getUserName;
//        if (user_name.empty() || checkingForStringExistence(user_name, get_name) != UNSUCCESSFUL)
//        {
//            std::cout << std::endl << RED << "Please change name!" << RESET << std::endl;
//        }
//        else
//        {
//            isOK = true;
//        }
//    }
//}
//
// auto Application::createAccount_inputLogin(std::string& user_login) const -> void
//{
//    auto isOK{false};
//
//    while (!isOK)
//    {
//        std::cout << std::endl << "Login(max " << MAX_INPUT_SIZE << " letters): ";
//        std::cout << BOLDGREEN;
//        Utils::getString(user_login, MAX_INPUT_SIZE);
//        std::cout << RESET;
//        const std::string& (User::*get_login)() const = &User::getUserLogin;
//        if (user_login.empty() || checkingForStringExistence(user_login, get_login) != UNSUCCESSFUL)
//        {
//            std::cout << std::endl << RED << "Please change login." << RESET;
//        }
//        else
//        {
//            isOK = true;
//        }
//    }
//}
//
// auto Application::createAccount_inputPassword(std::string& user_password) const -> void
//{
//    auto isOK{false};
//    while (!isOK)
//    {
//        // std::cout << std::endl << "Password(max " << MAX_INPUT_SIZE << " letters): ";
//        // std::cout << BOLDGREEN;
//        // Utils::getBoundedString(user_password, MAX_INPUT_SIZE, true);
//        // std::cout << RESET;
//
//        Utils::getPassword(user_password, "Password(max " + std::to_string(MAX_INPUT_SIZE) + " letters): ");
//
//        if (user_password.empty()) continue;
//
//        // std::cout << std::endl << "Re-enter your password: ";
//        // std::cout << BOLDGREEN;
//
//        std::string check_user_password;
//        //        Utils::getBoundedString(check_user_password, MAX_INPUT_SIZE, true);
//
//        Utils::getPassword(check_user_password, "Re-enter your password: ");
//
//        // std::cout << RESET;
//        if (user_password != check_user_password)
//        {
//            std::cout << std::endl << RED << "Password don't match!" << RESET;
//        }
//        else
//        {
//            isOK = true;
//        }
//    }
//}
//
// auto Application::signIn() -> int
//{
//    std::cout << std::endl;
//    std::cout << BOLDYELLOW << UNDER_LINE << "Sign In:" << RESET << std::endl;
//
//    std::string user_login{};
//    std::string user_password{};
//    while (true)
//    {
//        auto index{signIn_inputLogin(user_login)};
//
//        signIn_inputPassword(user_password);
//
//        if (index != UNSUCCESSFUL)
//        {
//            auto it = _password_hash.find(user_login);
//            std::shared_ptr<PasswordHash> password_hash = sha1(user_password, it->second->getSalt());
//            auto password_match{true};
//            for (auto i{0}; i < SHA1HASHLENGTHUINTS; ++i)
//            {
//                if (it->second->getHash() == password_hash->getHash()) continue;
//                password_match = false;
//                break;
//            }
//            if (password_match)
//            {
//                selectCommonOrPrivate(_user_array[index]);
//                return index;
//            }
//        }
//
//        std::cout << std::endl << RED << "Login or Password don't match!" << std::endl;
//        std::cout << BOLDYELLOW << std::endl << "Try again?(Y/N):" << BOLDGREEN;
//        if (!Utils::isOKSelect()) return UNSUCCESSFUL;
//    }
//}
//
// auto Application::signIn_inputLogin(std::string& user_login) const -> int
//{
//    std::cout << RESET << "Login:";
//    std::cout << BOLDGREEN;
//
//    Utils::getString(user_login);
//
//    std::cout << RESET;
//    const std::string& (User::*get_login)() const = &User::getUserLogin;
//    return checkingForStringExistence(user_login, get_login);
//}
// auto Application::signIn_inputPassword(std::string& user_password) const -> void
//{
//    // std::cout << RESET << "Password:";
//    // std::cout << BOLDGREEN;
//    // Utils::getBoundedString(user_password, MAX_INPUT_SIZE, true);
//    // std::cout << RESET << std::endl;
//
//    Utils::getPassword(user_password, "Password: ");
//}
//
// auto Application::selectCommonOrPrivate(const std::shared_ptr<User>& user) -> int
//{
//    auto isContinue{true};
//    while (isContinue)
//    {
//        std::string menu_arr[] = {"Select chat type:", "Common chat", "Private chat", "Sign Out"};
//
//        auto user_number{_new_messages_array[user->getUserID()]->usersNumber()};
//        if (user_number)  // if exist new message for this user
//        {
//            menu_arr[2] = BOLDYELLOW + menu_arr[2] + RESET + GREEN + "(New message(s) from " + std::to_string(user_number) + " user(s))" +
//                          RESET;  // menu_arr[2] = "Private chat"
//        }
//
//        auto menu_item{menu(menu_arr, 4)};
//
//        switch (menu_item)
//        {
//            case 1: commonChat(user); break;
//            case 2: privateMenu(user); break;
//            default: isContinue = false; break;
//        }
//    }
//
//    return 0;
//}
//
// auto Application::commonChat(const std::shared_ptr<User>& user) const -> int
//{
//    auto isContinue{true};
//    while (isContinue)
//    {
//        std::string menu_arr[]{"Common Chat:", "View chat", "Add message", "Edit message", "Delete message", "Exit"};
//        auto menu_item{menu(menu_arr, 6)};
//
//        switch (menu_item)
//        {
//            case 1:
//                std::cout << std::endl;
//                //               _common_chat->printMessages(0, _common_chat->getCurrentMessageNum());
//                break;
//            case 2: commonChat_addMessage(user); break;
//            case 3: commonChat_editMessage(user); break;
//            case 4: commonChat_deleteMessage(user); break;
//            default: isContinue = false; break;
//        }
//    }
//    return SUCCESSFUL;
//}
//
// auto Application::commonChat_addMessage(const std::shared_ptr<User>& user) const -> void
//{
////    _common_chat->addMessage(user);
//}
//
// auto Application::commonChat_editMessage(const std::shared_ptr<User>& user) const -> void
//{
//    std::cout << std::endl << YELLOW << "Select message number for editing: " << BOLDGREEN;
//    int message_number{Utils::inputIntegerValue()};
//    std::cout << RESET;
//    _common_chat->editMessage(user, message_number - 1);  // array's indices begin from 0, Output indices begin from 1
//}
//
// auto Application::commonChat_deleteMessage(const std::shared_ptr<User>& user) const -> void
//{
//    std::cout << std::endl << YELLOW << "Select message number for deleting: " << BOLDGREEN;
//    int message_number{Utils::inputIntegerValue()};
//    std::cout << RESET;
//    _common_chat->deleteMessage(user, message_number - 1);  // array's indices begin from 0, Output indices begin from 1
//}
//
// auto Application::privateMenu(const std::shared_ptr<User>& user) -> int
//{
//    auto isContinue{true};
//    while (isContinue)
//    {
//        printNewMessagesUsers(user);
//
//        std::string menu_arr[]{"Private Chat:", "View chat users names", "Select target user by name", "Select target user by ID", "Exit"};
//
//        auto menu_item{menu(menu_arr, 5)};
//
//        switch (menu_item)
//        {
//            case 1: privateMenu_viewUsersNames(); break;
//            case 2:
//            {
//                auto index{0};
//                if ((index = privateMenu_selectByName(user)) != UNSUCCESSFUL) privateChat(user, _user_array[index]);
//            }
//            break;
//            case 3: privateMenu_selectByID(user); break;
//            default: isContinue = false; break;
//        }
//    }
//    return 0;
//}
//
// auto Application::privateMenu_viewUsersNames() const -> void
//{
//    std::cout << std::endl;
//    std::cout << BOLDGREEN << std::setw(5) << std::setfill(' ') << std::right << "ID"
//              << "." << BOLDYELLOW << std::setw(MAX_INPUT_SIZE) << std::setfill(' ') << std::left << "User Name" << std::endl;
//
//    for (auto i{0}; i < _current_user_number; ++i)
//    {
//        std::cout << BOLDGREEN << std::setw(5) << std::setfill(' ') << std::right << i + 1 << "." << BOLDYELLOW << std::setw(MAX_INPUT_SIZE)
//                  << std::setfill(' ') << std::left << _user_array[i]->getUserName() << std::endl;  // array's indices begin from 0, Output indices begin from
//                  1
//        if (!((i + 1) % LINE_TO_PAGE))
//        {
//            std::cout << std::endl << RESET << YELLOW << "Press Enter for continue...";
//            std::cin.get();  //  Suspend via LINE_TO_PAGE lines
//        }
//    }
//    std::cout << RESET;
//}
// auto Application::privateMenu_selectByName(const std::shared_ptr<User>& user) const -> int
//{
//    auto index{UNSUCCESSFUL};
//    auto isOK{false};
//    while (!isOK)
//    {
//        std::cout << std::endl << RESET << YELLOW << "Input target user name: " << BOLDYELLOW;
//        std::string user_name;
//        std::cin >> user_name;
//        std::cout << RESET;
//        const std::string& (User::*get_name)() const = &User::getUserName;
//        if ((index = checkingForStringExistence(user_name, get_name)) == UNSUCCESSFUL)
//        {
//            std::cout << RED << "User don't exist!" << std::endl;
//            std::cout << std::endl << BOLDYELLOW << "Try again?(Y/N):" << BOLDGREEN;
//            if (!Utils::isOKSelect()) return UNSUCCESSFUL;
//            continue;
//        }
//        isOK = true;
//    }
//    return index;
//}
// auto Application::privateMenu_selectByID(const std::shared_ptr<User>& user) -> void
//{
//    std::cout << std::endl << RESET << YELLOW << "Input target user ID: " << BOLDGREEN;
//    auto index{Utils::inputIntegerValue()};
//    std::cout << RESET;
//    try
//    {
//        privateChat(user, _user_array.at(index - 1));  // array's indices begin from 0, Output indices begin from 1
//    }
//    catch (std::exception& e)
//    {
//        std::cout << BOLDRED << "Exception: " << e.what() << RESET << std::endl;
//    }
//}
//
// auto Application::printNewMessagesUsers(const std::shared_ptr<User>& user) -> void
//{
//    auto new_message{_new_messages_array[user->getUserID()]};
//    auto user_number{new_message->usersNumber()};
//    if (user_number)
//    {
//        std::cout << std::endl;
//        std::cout << BOLDYELLOW << UNDER_LINE << "User sended new message(s):" << RESET << std::endl;
//        std::cout << std::endl;
//        std::cout << BOLDGREEN << std::setw(5) << std::setfill(' ') << std::right << "ID"
//                  << "." << BOLDYELLOW << std::setw(MAX_INPUT_SIZE) << std::setfill(' ') << std::left << "User Name" << std::endl;
//
//        for (auto i{0u}; i < user_number; ++i)
//        {
//            auto userID{new_message->getUserID(i)};
//            auto msg_vector{new_message->getMessages(userID)};
//            auto msg_number{msg_vector.size()};
//            std::cout << BOLDGREEN << std::setw(5) << std::setfill(' ') << std::right << userID + 1 << "." << BOLDYELLOW << std::setw(MAX_INPUT_SIZE)
//                      << std::setfill(' ') << std::left << _user_array[userID]->getUserName() << RESET << GREEN << "(" << msg_number << " new message(s))"
//                      << std::endl;  // array's indices begin from 0, Output indices begin from 1
//        }
//    }
//}
//
// auto Application::privateChat(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user) -> int
//{
//    auto isContinue{true};
//
//    auto currentChat{getPrivateChat(source_user, target_user)};
//
//    while (isContinue)
//    {
//        std::string menu_arr[]{"Private Chat:", "View chat", "Add message", "Edit message", "Delete message", "Exit"};
//
//        auto menu_item{menu(menu_arr, 6)};
//
//        switch (menu_item)
//        {
//            case 1:
//                if (currentChat.get()->isInitialized())
//                {
//                    std::cout << std::endl;
//                    //                   currentChat->printMessages(0, currentChat->getCurrentMessageNum());
//
//                    auto new_message{_new_messages_array[source_user->getUserID()]};
//                    auto msg_vector{new_message->getMessages(target_user->getUserID())};
//                    auto msg_number{msg_vector.size()};
//                    if (msg_number)
//                    {
//                        new_message->removeAllMessages(target_user->getUserID());
//                    }
//                }
//                break;
//            case 2: privateChat_addMessage(source_user, target_user, currentChat); break;
//            case 3: privateChat_editMessage(source_user, target_user, currentChat); break;
//            case 4: privateChat_deleteMessage(source_user, target_user, currentChat); break;
//            default: isContinue = false; break;
//        }
//    }
//    return 0;
//}
//
// auto Application::privateChat_addMessage(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user, std::shared_ptr<Chat>& chat)
//    -> void
//{
//    if (!chat->isInitialized())
//    {
//        chat = std::make_shared<Chat>();
//        long long first_userID{source_user->getUserID()};
//        long long second_userID{target_user->getUserID()};
//        auto isSwap(Utils::minToMaxOrder(first_userID, second_userID));
//
//        long long mapKey{(static_cast<long long>(first_userID) << 32) + second_userID};  // Create value for key value
//
//        if (isSwap)
//        {
//            chat->setFirstUser(target_user);
//            chat->setSecondUser(source_user);
//        }
//        else
//        {
//            chat->setFirstUser(source_user);
//            chat->setSecondUser(target_user);
//        }
//        _private_chat_array[mapKey] = chat;
//        ++_current_chat_number;
//        chat->setInitialized(true);
//    }
//    //auto message{chat->addMessage(source_user)};
//    //if (!message->isInitialized()) return;
//    //auto index{target_user->getUserID()};
//    //_new_messages_array[index]->addNewMessage(message);
//}
// auto Application::privateChat_editMessage(
//    const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user, const std::shared_ptr<Chat>& chat) const -> void
//{
//    std::cout << std::endl << RESET << YELLOW << "Select message number for editing: " << BOLDGREEN;
//    int message_number{Utils::inputIntegerValue()};
//    std::cout << RESET;
//    if (chat->isInitialized())
//    {
//        auto message{chat->editMessage(source_user, message_number - 1)};  // array's indices begin from 0, Output indices begin from 1
//        if (!message->isInitialized()) return;
//
//        auto index{target_user->getUserID()};
//        _new_messages_array[index]->addNewMessage(message);
//    }
//}
//
// auto Application::privateChat_deleteMessage(
//    const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user, const std::shared_ptr<Chat>& chat) const -> void
//{
//    std::cout << std::endl << RESET << YELLOW << "Select message number for deleting: " << BOLDGREEN;
//    int message_number{Utils::inputIntegerValue()};
//    std::cout << RESET;
//    if (chat->isInitialized())
//    {
//        auto message{chat->deleteMessage(source_user, message_number - 1)};  // array's indices begin from 0, Output indices begin from 1
//        if (!message->isInitialized()) return;
//
//        auto index{target_user->getUserID()};
//        _new_messages_array[index]->removeNewMessage(message);
//    }
//}
//
// auto Application::getPrivateChat(const std::shared_ptr<User>& source_user, const std::shared_ptr<User>& target_user) const -> const std::shared_ptr<Chat>
//{
//    long long first_userID{source_user->getUserID()};
//    long long second_userID{target_user->getUserID()};
//
//    Utils::minToMaxOrder(first_userID, second_userID);
//
//    long long searchID{(static_cast<long long>(first_userID) << 32) + second_userID};  // Create value for search
//
//    auto it = _private_chat_array.begin();
//
//    for (; it != _private_chat_array.end(); ++it)
//    {
//        if (it->first == searchID) return it->second;
//    }
//
//    return std::make_shared<Chat>();
//}
//
// auto Application::checkingForStringExistence(const std::string& string, const std::string& (User::*get)() const) const -> int
//{
//    for (auto i{0}; i < _current_user_number; ++i)
//    {
//        if (string == (_user_array[i].get()->*get)()) return i;
//    }
//    return UNSUCCESSFUL;
//}

auto Application::menu(std::string* string_arr, int size) const -> int
{
    if (size <= 0) return UNSUCCESSFUL;

    std::cout << std::endl;
    std::cout << BOLDYELLOW << UNDER_LINE << string_arr[0] << RESET << std::endl;  // index 0 is Menu Name

    for (auto i{1}; i < size; ++i)
    {
        std::cout << BOLDGREEN << i << "." << RESET << string_arr[i] << std::endl;
    }
    std::cout << YELLOW << "Your choice?: " << BOLDGREEN;
    int menu_item{Utils::inputIntegerValue()};
    std::cout << RESET;

    return menu_item;
}

// auto Application::save() -> void
//{
//    if (!saveUserArray()) return;
//
//    savePasswordHash();
//
//    saveChats();
//
//    saveNewMessages();
//}
//
// auto Application::saveUserArray() const -> bool
//{
//    // Save vector<User>
//    File file_user(_self_path + std::string("User.txt"), std::fstream::out);
//    if (file_user.getError()) return false;
//
//    file_user.write(_user_array.size());
//
//    for (auto i{0}; i < _user_array.size(); ++i)
//    {
//        file_user.write(_user_array[i]->getUserName());
//        file_user.write(_user_array[i]->getUserLogin());
//        file_user.write(_user_array[i]->getUserID());
//    }
//    return true;
//}
//
// auto Application::savePasswordHash() -> void
//{
//    // Save _password_hash
//    File file_hash(_self_path + std::string("UserHash.txt"), std::fstream::out);
//    for (auto i{0}; i < _user_array.size(); ++i)
//    {
//        file_hash.write(_password_hash[_user_array[i]->getUserLogin()]->getSalt());
//        Hash hash = _password_hash[_user_array[i]->getUserLogin()]->getHash();
//        file_hash.write(hash._A);
//        file_hash.write(hash._B);
//        file_hash.write(hash._C);
//        file_hash.write(hash._D);
//        file_hash.write(hash._E);
//    }
//}
//
// auto Application::saveChats() const -> void
//{
//    // Save Chats (Common and Privats)
//    File file_chat(_self_path + std::string("Chat.txt"), std::fstream::out);
//
//    _common_chat->save(file_chat);
//
//    file_chat.write(_private_chat_array.size());
//
//    for (auto ch : _private_chat_array)
//    {
//        ch.second->save(file_chat);
//    }
//}
//
// auto Application::saveNewMessages() -> void
//{
//    // Save New Messages (Common and Privats)
//    File file_newmsg(_self_path + std::string("NewMessages.txt"), std::fstream::out);
//
//    auto target_users_number{_new_messages_array.size()};
//    file_newmsg.write(target_users_number);
//
//    for (auto i{0u}; i < target_users_number; ++i)
//    {
//        auto newMessage{_new_messages_array[i]};
//        auto source_users_number{newMessage->usersNumber()};
//        if (!source_users_number) continue;      // if  user has no new messages
//        file_newmsg.write(i);                    // save target userID
//        file_newmsg.write(source_users_number);  // save initiator numbers
//
//        for (auto j{0u}; j < source_users_number; ++j)
//        {
//
//            auto userID{newMessage->getUserID(j)};
//            long long first_userID{i};
//            long long second_userID{userID};
//            Utils::minToMaxOrder(first_userID, second_userID);
//            long long mapKey{(static_cast<long long>(first_userID) << 32) + second_userID};  // Create value for key value
//            file_newmsg.write(userID);                                                       // save initiator ID
//            auto message{newMessage->getMessages(userID)};                                   //
//            auto msg_number{message.size()};                                                 //
//            file_newmsg.write(msg_number);                                                   // msg number from initiator
//
//            for (auto k{0}; k < msg_number; ++k)
//            {
//                file_newmsg.write(_private_chat_array[mapKey]->getMessageIndex(message[k]));  // msg index in chat
//            }
//        }
//    }
//}
//
// auto Application::load() -> void
//{
//    if (!loadUserArray()) return;
//
//    loadPasswordHash();
//
//    loadChats();
//
//    loadNewMessages();
//}
//
//// Load vector<User>
// auto Application::loadUserArray() -> bool
//{
//    File file_user(_self_path + std::string("User.txt"), std::fstream::in);
//
//    if (file_user.getError()) return false;
//
//    size_t user_count{0};
//    file_user.read(user_count);
//    _current_user_number = static_cast<int>(user_count);
//
//    for (auto i{0}; i < _current_user_number; ++i)
//    {
//        std::string name{};
//        file_user.read(name);
//        std::string login{};
//        file_user.read(login);
//        int userID{-1};
//        file_user.read(userID);
//
//        std::shared_ptr<User> user = std::make_shared<User>(name, login, userID);
//
//        _user_array.push_back(user);
//
//        _new_messages_array.push_back(std::make_shared<NewMessages>());  // TODO need loading!!!
//    }
//    return true;
//}
//
//// Load Password Hash
// auto Application::loadPasswordHash() -> void
//{
//    File file_hash(_self_path + std::string("UserHash.txt"), std::fstream::in);
//    for (auto i{0}; i < _user_array.size(); ++i)
//    {
//        std::string salt{};
//        file_hash.read(salt);
//        Hash hash;
//        file_hash.read(hash._A);
//        file_hash.read(hash._B);
//        file_hash.read(hash._C);
//        file_hash.read(hash._D);
//        file_hash.read(hash._E);
//        _password_hash[_user_array[i]->getUserLogin()] = std::make_shared<PasswordHash>(hash, salt);
//    }
//}
//
//// Load Chats (Common and Privats)
// auto Application::loadChats() -> void
//{
//    File file_chat(_self_path + std::string("Chat.txt"), std::fstream::in);
//
//    int user1{0}, user2{0};
//    file_chat.read(user1);
//    file_chat.read(user2);
//    if (user1 > 0 || user2 > 0) return;  // Chat.txt begin from -1 -1 (Common chat don't have users)
//
//    _common_chat.get()->load(file_chat, _user_array);
//
//    size_t private_chats_number{0};
//    file_chat.read(private_chats_number);
//
//    for (auto i{0}; i < private_chats_number; ++i)
//    {
//        int first_userID{0}, second_userID{0};
//        file_chat.read(first_userID);
//        file_chat.read(second_userID);
//
//        long long keyID{(static_cast<long long>(first_userID) << 32) + second_userID};
//
//        _private_chat_array[keyID] = std::make_shared<Chat>();
//
//        _private_chat_array[keyID]->setFirstUser(_user_array[first_userID]);
//        _private_chat_array[keyID]->setSecondUser(_user_array[second_userID]);
//
//        _private_chat_array[keyID]->load(file_chat, _user_array);
//    }
//}
//
// auto Application::loadNewMessages() -> void
//{
//    // Load New Messages (Common and Privats)
//    File file_newmsg(_self_path + std::string("NewMessages.txt"), std::fstream::in);
//    if (file_newmsg.getError()) return;
//    auto user_number{0};
//    file_newmsg.read(user_number);
//    while (!file_newmsg.getStream().eof()) /* for (auto i{0}; i < user_number; ++i)*/
//    {
//        auto target_userID{0};
//        file_newmsg.read(target_userID);
//        auto users_with_new_msg_number{0};
//        file_newmsg.read(users_with_new_msg_number);
//        for (auto j{0}; j < users_with_new_msg_number; ++j)
//        {
//            auto init_userID{0};
//            file_newmsg.read(init_userID);
//
//            long long first_userID{target_userID};
//            long long second_userID{init_userID};
//
//            Utils::minToMaxOrder(first_userID, second_userID);
//            long long mapKey{(static_cast<long long>(first_userID) << 32) + second_userID};  // Create value for key value
//            auto chat{_private_chat_array[mapKey]};
//            auto msg_number{0};
//            file_newmsg.read(msg_number);
//            for (auto k{0}; k < msg_number; ++k)
//            {
//                auto msg_index{0};
//                file_newmsg.read(msg_index);
//                auto message{chat->getMessageByIndex(msg_index)};
//                _new_messages_array[target_userID]->addNewMessage(message);
//            }
//        }
//    }
//    return;
//}
auto Application::reaction(char* message, int thread_num) -> void
{
    auto code{-1};
    getFromBuffer(message, 0, code);
    //   std::cout << "CODE: " << code << std::endl;
    try
    {
        auto code_operation = static_cast<OperationCode>(code);
        switch (code_operation)
        {
            case OperationCode::STOP: onStop(message, thread_num); break;
            case OperationCode::CHECK_SIZE: onCheckSize(message, thread_num); break;
            case OperationCode::CHECK_EMAIL: onCheckEmail(message, thread_num); break;
            case OperationCode::CHECK_LOGIN: onCheckLogin(message, thread_num); break;
            case OperationCode::REGISTRATION: onRegistration(message, thread_num); break;
            case OperationCode::SIGN_IN: onSignIn(message, thread_num); break;
            case OperationCode::COMMON_CHAT_GET_MESSAGES: onCommonChatGetMessages(message, thread_num); break;
            case OperationCode::COMMON_CHAT_ADD_MESSAGE: onCommonChatAddMessage(message, thread_num); break;
            case OperationCode::COMMON_CHAT_CHECK_MESSAGE: onCommonChatCheckMessage(message, thread_num); break;
            case OperationCode::COMMON_CHAT_EDIT_MESSAGE: onCommonChatEditMessage(message, thread_num); break;
            case OperationCode::COMMON_CHAT_DELETE_MESSAGE: onCommonChatDeleteMessage(message, thread_num); break;
            case OperationCode::NEW_MESSAGES_IN_COMMON_CHAT: onNewMessagesInCommonChat(message, thread_num); break;
            case OperationCode::VIEW_USERS_ID_NAME_SURNAME: onViewUsersIDNameSurname(message, thread_num); break;
            default: return onError(message, thread_num); break;
        }
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Invalid argument!" << std::endl;
        const char* inv_arg = "Invalid argument!";
        _server->getMessageSizeRef(thread_num) = 0;
        addToBuffer(message, _server->getMessageSizeRef(thread_num), inv_arg, strlen(inv_arg));
    }
}

auto Application::onCheckSize(char* message, int thread_num) const -> void
{
    auto message_length{-1};
    getFromBuffer(message, sizeof(int), message_length);

    _server->setBufferSize(thread_num, message_length + HEADER_SIZE);
    _server->getMessageSizeRef(thread_num) = 2 * sizeof(message_length);  // first int CHECK_SIZE
    _server->setMsgFromClientSize(message_length, thread_num);
}

auto Application::onCheckEmail(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            checkEmail(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        default: return onError(message, thread_num); break;
    }
}

auto Application::onCheckLogin(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            checkLogin(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        default: return onError(message, thread_num); break;
    }
}

auto Application::onRegistration(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            registration(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onSignIn(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            signin(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

// auto Application::onNewMessages(const std::string& in_message, std::string& out_message, int thread_num) -> void
//{
//    std::string code_operation_string;
//    std::string string, user_index;
//    std::stringstream stream(in_message);
//
//    stream >> code_operation_string >> code_operation_string >> user_index;
//    string = user_index;
//
//    auto code_operation = static_cast<OperationCode>(std::stoi(code_operation_string));
//    switch (code_operation)
//    {
//        case OperationCode::CHECK_SIZE:
//        {
//            auto msg{newmessages(string)};
//            _server->setCashMessage(msg, thread_num);
//            out_message = std::to_string(static_cast<int>(OperationCode::CHECK_SIZE)) + " " + std::to_string(msg.size() + HEADER_SIZE);
//            break;
//        }
//        case OperationCode::READY: out_message = _server->getCashMessage(thread_num); break;
//        default: return onError(out_message); break;
//    }
//}
//
// auto Application::onGetNumberMessagesInChat(const std::string& in_message, std::string& out_message, int thread_num) -> void
//{
//    std::string code_operation_string;
//    std::string chat_id_string, first_user, second_user;
//    std::stringstream stream(in_message);
//
//    stream >> code_operation_string >> code_operation_string >> first_user >> second_user;
//    chat_id_string = first_user + DELIMITER + second_user;
//
//    auto code_operation = static_cast<OperationCode>(std::stoi(code_operation_string));
//    switch (code_operation)
//    {
//        case OperationCode::CHECK_SIZE:
//        {
//            auto msg{getNumberMessagesInChat(chat_id_string)};
//            _server->setCashMessage(msg, thread_num);
//            out_message = std::to_string(static_cast<int>(OperationCode::CHECK_SIZE)) + DELIMITER + std::to_string(msg.size() + HEADER_SIZE);
//            break;
//        }
//        case OperationCode::READY: out_message = _server->getCashMessage(thread_num); break;
//        default: return onError(out_message); break;
//    }
//}

auto Application::onCommonChatGetMessages(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            commonChatGetMessages(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onCommonChatAddMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            commonChatAddMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onCommonChatCheckMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            commonChatCheckMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onCommonChatEditMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            commonChatEditMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onCommonChatDeleteMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            commonChatDeleteMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onNewMessagesInCommonChat(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            newMessagesInCommonChat(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onViewUsersIDNameSurname(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            viewUsersIDNameSurname(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
            _server->setBufferSize(thread_num, _server->getCashMessageSizeRef(thread_num));
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::CHECK_SIZE));
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        case OperationCode::READY:
        {
            _server->getMessageSizeRef(thread_num) = 0;
            addToBuffer(message, _server->getMessageSizeRef(thread_num), _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num));
            break;
        }
        default: return onError(message, thread_num); break;
    }
}

auto Application::onStop(char* message, int thread_num) -> void
{
    // std::string user_id{(message + sizeof(int))};
    // std::cout << "User ID: " << user_id << "!" << std::endl;

    // std::string query_str = "UPDATE Users SET viewdate = now() WHERE id = '" + std::string(message + sizeof(int)) + "'";
    //_data_base->query(query_str.c_str());
}

auto Application::onError(char* message, int thread_num) const -> void
{
    addToBuffer(message, _server->getMessageSizeRef(thread_num), static_cast<int>(OperationCode::NET_ERROR));
    const char* err = "ERROR";
    addToBuffer(message, _server->getMessageSizeRef(thread_num), err, strlen(err));
}

auto Application::checkEmail(char* email, size_t email_size, int thread_num) -> void
{
    std::string mail{email};
    std::string email_query = "SELECT '" + mail + "' LIKE '%@%.%'";
    _data_base->query(email_query.c_str());

    int row_num{0};
    int column_num{0};
    std::string query_res{};
    _data_base->getQueryResult(query_res, row_num, column_num);
    std::string is_email_pattern_valid{query_res.c_str()};

    const char* result{nullptr};
    if (is_email_pattern_valid == "0")
    {
        result = "ERROR";
    }
    else
    {
        std::string query_str = "SELECT id FROM Users WHERE email = lower('" + std::string(email) + "')";
        std::string query_result{};
        int row_num{0};
        int column_num{0};

        _data_base->query(query_str.c_str());
        query_result.reserve(1024);
        _data_base->getQueryResult(query_result, row_num, column_num);

        if (!row_num)
            result = "OK";
        else
            result = "ERROR";
    }
    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::checkLogin(char* login, size_t login_size, int thread_num) -> void
{

    login[login_size] = '\0';
    std::string query_str = "SELECT id FROM Users WHERE login = lower('" + std::string(login) + "')";

    //   std::cout << login << std::endl;

    std::string query_result{};
    int row_num{0};
    int column_num{0};

    _data_base->query(query_str.c_str());
    query_result.reserve(1024);
    _data_base->getQueryResult(query_result, row_num, column_num);

    const char* result{nullptr};
    if (!row_num)
        result = "OK";
    else
        result = "ERROR";

    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::registration(char* regdata, size_t regdata_size, int thread_num) -> void
{
    auto data_ptr{regdata};
    std::string query_str = "INSERT INTO Users(name, surname, login, email, regdate, viewdate) VALUES (";
    std::string login{};
    std::string email{};
    for (auto i{0}; i < 4; ++i)
    {
        if (i == 2) login = data_ptr;  // save login for future query
        auto length{strlen(data_ptr)};
        query_str += "'";
        query_str += data_ptr;
        query_str += "', ";
        data_ptr += length + 1;  // pointer in the next word
    }
    query_str += "now(),'2020-12-31 10:10:10')";
    std::string password = data_ptr;

    const char* result{nullptr};

    if (!_data_base->query(query_str.c_str()))
    {
        query_str = "SELECT id FROM Users WHERE login = '" + login + "'";
        _data_base->query(query_str.c_str());
        std::string query_result{};
        int row_num{0};
        int column_num{0};
        _data_base->getQueryResult(query_result, row_num, column_num);

        std::string id{query_result.c_str()};
        std::string salt{Utils::getSalt()};
        password += salt;
        query_str = "INSERT INTO Hash (id, hash, salt) VALUES(" + id + ", md5('" + password + "'),'" + salt + "');";
        _data_base->query(query_str.c_str());

        // auto err_ptr{_data_base->getMySQLError()};
        // std::cout << err_ptr << std::endl;

        result = RETURN_OK.c_str();
    }
    else
    {
        auto err_ptr{_data_base->getMySQLError()};
        std::cout << err_ptr << std::endl;

        if (strstr(err_ptr, "login"))
        {
            result = "LOGIN";
        }
        else if (strstr(err_ptr, "email"))
        {
            result = "EMAIL";
        }
        else
        {
            result = RETURN_ERROR.c_str();
        }
    }

    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::signin(char* signin_data, size_t signin_gdata_size, int thread_num) -> void
{
    auto data_ptr{signin_data};
    std::string login{signin_data};
    std::string password{data_ptr + login.size() + 1};

    std::string query_str = "SELECT id FROM Users WHERE login ='" + login + "'";

    _data_base->query(query_str.c_str());

    std::string query_result{};
    int row_num{0};
    int column_num{0};
    _data_base->getQueryResult(query_result, row_num, column_num);

    const char* result{nullptr};
    if (row_num)
    {
        result = query_result.c_str();  // user ID

        std::string id{query_result.c_str()};
        query_str = "SELECT hash,salt FROM Hash WHERE id ='" + id + "'";
        _data_base->query(query_str.c_str());

        std::string query_hash_salt{};
        _data_base->getQueryResult(query_hash_salt, row_num, column_num);
        auto data_ptr{query_hash_salt.c_str()};
        std::string hash{data_ptr};
        std::string salt{data_ptr + hash.size() + 1};

        query_str = "SELECT md5('" + password + salt + "')";
        _data_base->query(query_str.c_str());
        std::string query_check_hash{};
        _data_base->getQueryResult(query_check_hash, row_num, column_num);
        std::string check_hash{query_check_hash.c_str()};

        if (check_hash != hash) result = RETURN_ERROR.c_str();

        _connected_user_id[thread_num] = std::move(id);

        // auto err_ptr{_data_base->getMySQLError()};
        // std::cout << err_ptr << std::endl;
    }
    else
    {
        result = RETURN_ERROR.c_str();
    }

    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::commonChatGetMessages(char* data, size_t data_size, int thread_num) -> void
{
    std::string query_str = "SELECT viewdate FROM Users WHERE id ='" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_str.c_str());

    std::string query_viewdate{};
    int row_num{0};
    int column_num{0};
    _data_base->getQueryResult(query_viewdate, row_num, column_num);
    std::string lastview{query_viewdate.c_str()};
    query_str = "SELECT c.id, name, surname, user_id, message, creation_date, edited, editing_date "
                "FROM  Users AS u JOIN CommonMessages AS c ON u.id = c.user_id WHERE creation_date <= '" +
                lastview + "'";
    _data_base->query(query_str.c_str());

    std::string old_msg{};
    int old_row_num{0};
    int old_column_num{0};
    _data_base->getQueryResult(old_msg, old_row_num, old_column_num);
    query_str = "SELECT c.id, name, surname, user_id, message, creation_date, edited, editing_date "
                "FROM  Users AS u JOIN CommonMessages AS c ON u.id = c.user_id WHERE creation_date > '" +
                lastview + "'";
    _data_base->query(query_str.c_str());

    std::string new_msg{};
    auto new_row_num{0};
    auto new_column_num{0};
    _data_base->getQueryResult(new_msg, new_row_num, new_column_num);

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    _server->resizeCashMessageBuffer(thread_num, old_msg.size() + new_msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::COMMON_CHAT_GET_MESSAGES));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_column_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_column_num);
    if (old_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_msg.c_str(), old_msg.size());
    if (new_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_msg.c_str(), new_msg.size());

    query_str = "UPDATE Users SET viewdate = now() WHERE id = '" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_str.c_str());
}

auto Application::commonChatAddMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string chat_message{message};

    std::string query_str = "INSERT INTO CommonMessages (user_id, message, creation_date)"
                            "VALUES('" +
                            _connected_user_id[thread_num] + "', '" + message + "', now())";

    _data_base->query(query_str.c_str());
    const char* result{nullptr};
    if (auto err_ptr{_data_base->getMySQLError()})
    {
        result = RETURN_ERROR.c_str();
        // std::cout << err_ptr << std::endl;
    }
    else
    {
        result = RETURN_OK.c_str();
    }
    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::commonChatCheckMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string message_id{message};

    std::string query_msg = "SELECT c.id, name, surname, user_id, message, creation_date, edited, editing_date "
                            "FROM  Users AS u JOIN CommonMessages AS c ON u.id = c.user_id WHERE user_id = '" +
                            _connected_user_id[thread_num] + "' AND  c.id = '" + message_id + "'";
    _data_base->query(query_msg.c_str());

    std::string msg{};
    int msg_row_num{0};
    int msg_column_num{0};
    _data_base->getQueryResult(msg, msg_row_num, msg_column_num);

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    _server->resizeCashMessageBuffer(thread_num, msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::COMMON_CHAT_CHECK_MESSAGE));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), msg_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), msg_column_num);
    if (msg_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), msg.c_str(), msg.size());
}

auto Application::commonChatEditMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string msg_id{message};
    std::string edited_message{message + msg_id.size() + 1};

    std::string query_msg = "UPDATE CommonMessages SET message = '" + edited_message + "', edited = 1, editing_date = now() WHERE id = '" + msg_id +
                            "' AND user_id = '" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_msg.c_str());

    auto err_ptr{_data_base->getMySQLError()};
    std::cout << err_ptr << std::endl;

    const char* result{nullptr};

    if (auto err_ptr{_data_base->getMySQLError()})
    {
        result = RETURN_ERROR.c_str();
        std::cout << err_ptr << std::endl;
    }
    else
    {
        result = RETURN_OK.c_str();
    }
    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::commonChatDeleteMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string msg_id{message};
    std::string edited_message{message + msg_id.size() + 1};

    std::string query_msg = "DELETE FROM CommonMessages WHERE id = '" + msg_id + "' AND user_id = '" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_msg.c_str());

    query_msg = "SET @count = 0";
    _data_base->query(query_msg.c_str());

    query_msg = "UPDATE CommonMessages SET CommonMessages.id = @count:= @count + 1";
    _data_base->query(query_msg.c_str());

    query_msg = "ALTER TABLE CommonMessages AUTO_INCREMENT = 1";
    _data_base->query(query_msg.c_str());

    auto err_ptr{_data_base->getMySQLError()};
    std::cout << err_ptr << std::endl;
}

auto Application::newMessagesInCommonChat(char* message, size_t message_size, int thread_num) -> void
{
    std::string query_str = "SELECT viewdate FROM Users WHERE id ='" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_str.c_str());

    std::string query_viewdate{};
    int row_num{0};
    int column_num{0};
    _data_base->getQueryResult(query_viewdate, row_num, column_num);
    std::string lastview{query_viewdate.c_str()};

    query_str = "SELECT c.user_id, count(*) "
                "FROM  Users AS u JOIN CommonMessages AS c ON u.id = c.user_id WHERE creation_date > '" +
                lastview + "' AND  c.user_id != '" + _connected_user_id[thread_num] + "' GROUP BY c.user_id";
    _data_base->query(query_str.c_str());

    auto err_ptr{_data_base->getMySQLError()};
    std::cout << err_ptr << std::endl;


    std::string new_msg{};
    auto new_row_num{0};
    auto new_column_num{0};
    _data_base->getQueryResult(new_msg, new_row_num, new_column_num);

    //std::cout << "new_row_num: " << new_row_num << std::endl;
    //std::cout << "new_column_num: " << new_column_num << std::endl;

    _server->resizeCashMessageBuffer(thread_num, new_msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(
        _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::NEW_MESSAGES_IN_COMMON_CHAT));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_column_num);
    if (new_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_msg.c_str(), new_msg.size());

}

auto Application::viewUsersIDNameSurname(char* message, size_t message_size, int thread_num) -> void 
{
    std::string query_str = "SELECT id, name, surname FROM Users";

    _data_base->query(query_str.c_str());

    std::string users_data{};
    auto data_row_num{0};
    auto data_column_num{0};
    _data_base->getQueryResult(users_data, data_row_num, data_column_num);

    _server->resizeCashMessageBuffer(thread_num, users_data.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(
        _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::VIEW_USERS_ID_NAME_SURNAME));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_column_num);
    if (data_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), users_data.c_str(), users_data.size());
}

auto Application::createDataBases() -> void
{
    //_data_base->query("DROP TABLE Users");
    _data_base->query("CREATE TABLE Users (id INT AUTO_INCREMENT PRIMARY KEY,"
                      "name VARCHAR(255) NOT NULL,"
                      "surname VARCHAR(255) NOT NULL,"
                      "login VARCHAR(255) NOT NULL UNIQUE,"
                      "email VARCHAR(255) NOT NULL UNIQUE,"
                      "regdate DATETIME NOT NULL,"
                      "viewdate DATETIME NOT NULL,"
                      "deleted INT DEFAULT 0)");

    //_data_base->query("DROP TABLE Hash");
    _data_base->query("CREATE TABLE Hash (id INT REFERENCES Users(id),"
                      "hash VARCHAR(64) NOT NULL,"
                      "salt VARCHAR(64) NOT NULL)");

    _data_base->query("CREATE TABLE Chat (id INT AUTO_INCREMENT PRIMARY KEY,"
                      "first_user_id INT REFERENCES Users(id),"
                      "second_user_id INT REFERENCES Users(id))");

    _data_base->query("CREATE TABLE Messages (id INT AUTO_INCREMENT PRIMARY KEY,"
                      "chat_id INT REFERENCES Chat(id),"
                      "user_id INT REFERENCES Users(id),"
                      "message VARCHAR(255) NOT NULL,"
                      "creation_date DATETIME NOT NULL,"
                      "edited INT DEFAULT 0,"
                      "editing_date DATETIME ,"
                      "status VARCHAR(100) NOT NULL CHECK( status IN ('done', 'in progress', 'delivery')))");

    _data_base->query("CREATE TABLE CommonMessages (id INT AUTO_INCREMENT PRIMARY KEY,"
                      "user_id INT REFERENCES Users(id),"
                      "message VARCHAR(255) NOT NULL,"
                      "creation_date DATETIME NOT NULL,"
                      "edited INT DEFAULT 0,"
                      "editing_date DATETIME )");

    // _data_base->query("INSERT INTO Users (name,surname,login,email,regdate)"
    //" VALUES"
    //"('Lida', 'Moroz', lower('Queen99'), lower('Lida_moroz@gmai.com'), now())");
    //"('Svetlana', 'Sokolova', lower('Sun7856'), lower('Sokolova@gmai.com'), now())");
    //"('Иван', 'Суржиков', lower('Inav777'), lower('Иван_Суржиков@почта.ру'), now())");
    //"('Проверка_Поля_на_достаточно_длинную_и_нестандартную_фамилию', 'такое_же_длинное_имя_с_нестандартными_символами_#!@$#?\_', lower('Login'),
    // lower('Почта@русская.Ру'), now())");
}

// auto Application::checkLogin(const std::string& user_login) -> const std::string
//{
//    const std::string& (User::*get_login)() const = &User::getUserLogin;
//    if (user_login.empty() || checkingForStringExistence(user_login, get_login) != UNSUCCESSFUL) return RETURN_ERROR;
//
//    return RETURN_OK;
//}
//
// auto Application::registration(const std::string& reg_string) -> const std::string
//{
//    std::stringstream stream(reg_string);
//    std::string name, login, password;
//    stream >> name >> login >> password;
//
//    const std::string& (User::*get_name)() const = &User::getUserName;
//    if (name.empty() || checkingForStringExistence(name, get_name) != UNSUCCESSFUL) return RETURN_ERROR + DELIMITER + "NAME";
//
//    const std::string& (User::*get_login)() const = &User::getUserLogin;
//    if (login.empty() || checkingForStringExistence(login, get_login) != UNSUCCESSFUL) return RETURN_ERROR + DELIMITER + "LOGIN";
//
//    _user_array.push_back(std::make_shared<User>(name, login, _current_user_number));
//
//    _new_messages_array.push_back(std::make_shared<NewMessages>());
//
//    const std::string salt = getSalt();
//    std::shared_ptr<PasswordHash> password_hash = sha1(password, salt);
//    _password_hash[login] = password_hash;
//    ++_current_user_number;
//
//    return RETURN_OK;
//}
//
// auto Application::signin(const std::string& signin_string, int thread_num) -> const std::string
//{
//    std::stringstream stream(signin_string);
//    std::string user_login, user_password;
//    stream >> user_login >> user_password;
//
//    const std::string& (User::*get_login)() const = &User::getUserLogin;
//    auto index{checkingForStringExistence(user_login, get_login)};
//
//    if (index != UNSUCCESSFUL)
//    {
//        auto it = _password_hash.find(user_login);
//        std::shared_ptr<PasswordHash> password_hash = sha1(user_password, it->second->getSalt());
//        auto password_match{true};
//        for (auto i{0}; i < SHA1HASHLENGTHUINTS; ++i)
//        {
//            if (it->second->getHash() == password_hash->getHash()) continue;
//            password_match = false;
//            break;
//        }
//        if (password_match)
//        {
//            _signed_user.push_back(thread_num);  // Set client authorization
//            return RETURN_OK + " " + std::to_string(index);
//        }
//    }
//    return RETURN_ERROR;
//}
//
// auto Application::newmessages(const std::string& user_index) -> const std::string
//{
//    int index{std::stoi(user_index)};
//    auto user_number{_new_messages_array[index]->usersNumber()};
//
//    return std::string(std::to_string(user_number));
//}
//
// auto Application::getNumberMessagesInChat(const std::string& msg_in_chat) -> std::string
//{
//    std::stringstream stream(msg_in_chat);
//    std::string first_user, second_user;
//    stream >> first_user >> second_user;
//
//    auto first_user_id{std::stoi(first_user)};
//    auto second_user_id{std::stoi(second_user)};
//
//    if (first_user_id == -1 && second_user_id == -1)  // Common Chat
//    {
//        return std::to_string(_common_chat->getCurrentMessageNum());
//    }
//
//    auto chat{getPrivateChat(_user_array[first_user_id], _user_array[second_user_id])};
//    return std::to_string(chat->getCurrentMessageNum());
//}
//
// auto Application::commonChatGetMessage(const std::string& message_index) -> std::string
//{
//    std::string common_chat_messages{RETURN_ERROR};
//    if (!_common_chat->getCurrentMessageNum()) return common_chat_messages;
//
//    auto index{std::stoi(message_index)};
//
//    std::stringstream stream{};
//    _common_chat->printMessage(index, stream);
//    common_chat_messages = stream.str();
//    return common_chat_messages;
//}
//
// auto Application::commonChatAddMessage(const std::string& message) -> std::string
//{
//    std::stringstream stream(message);
//    std::string user_id_string, message_string;
//    stream >> user_id_string >> message_string;
//
//    auto user_id{std::stoi(user_id_string)};
//    auto user_sptr{_user_array[user_id]};
//    auto msg_sptr{_common_chat->addMessage(user_sptr, message_string)};
//
//    if (msg_sptr->isInitialized()) return RETURN_OK;
//    return RETURN_ERROR;
//}
auto Application::addToBuffer(char* buffer, size_t& cur_msg_len, int value) const -> void
{
    auto length{sizeof(value)};
    auto char_ptr{reinterpret_cast<char*>(&value)};

    for (auto i{0}; i < length; ++i)
    {
        buffer[i + cur_msg_len] = char_ptr[i];
    }
    cur_msg_len += length;
}

auto Application::addToBuffer(char* buffer, size_t& cur_msg_len, const char* string, size_t str_len) const -> void
{
    for (auto i{0}; i < str_len; ++i)
    {
        buffer[i + cur_msg_len] = string[i];
    }
    cur_msg_len += str_len;
}

auto Application::getFromBuffer(const char* buffer, size_t shift, int& value) const -> void
{
    char val_buff[sizeof(value)];
    auto length{sizeof(value)};

    for (auto i{0}; i < length; ++i)
    {
        val_buff[i] = buffer[shift + i];
    }
    value = *(reinterpret_cast<int*>(val_buff));
}

auto Application::getFromBuffer(const char* buffer, size_t shift, char* string, size_t str_len) const -> void
{
    for (auto i{0}; i < str_len; ++i)
    {
        string[i] = buffer[shift + i];
    }
}
