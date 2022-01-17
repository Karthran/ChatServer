#ifdef _WIN32
#include <cstdio>
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#include <iostream>
#include <cstring>
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

    // std::string server_address{};
    // std::cout << "MySQL server address(press Enter for default 'localhost'): " << std::endl;
    // std::getline(std::cin, server_address);
    // if (!server_address.size()) server_address = "localhost";

    //   load();

    _data_base = std::make_unique<DataBase>("localhost", "root", "rksm", "testdb", 0);
    _data_base->init();
    _data_base->connect();

    createDataBases();

    _server = std::make_unique<Server>(this);
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
    return;
}

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

auto Application::reaction(char* message, int thread_num) -> void
{
    auto code{-1};
    getFromBuffer(message, 0, code);
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
            case OperationCode::PRIVATE_CHAT_CHECK_MESSAGE: onPrivateChatCheckMessage(message, thread_num); break;
            case OperationCode::COMMON_CHAT_EDIT_MESSAGE: onCommonChatEditMessage(message, thread_num); break;
            case OperationCode::COMMON_CHAT_DELETE_MESSAGE: onCommonChatDeleteMessage(message, thread_num); break;
            case OperationCode::NEW_MESSAGES_IN_COMMON_CHAT: onNewMessagesInCommonChat(message, thread_num); break;
            case OperationCode::NEW_MESSAGES_IN_PRIVATE_CHAT: onNewMessagesInPrivateChat(message, thread_num); break;
            case OperationCode::VIEW_USERS_ID_NAME_SURNAME: onViewUsersIDNameSurname(message, thread_num); break;
            case OperationCode::VIEW_USERS_WITH_NEW_MESSAGES: onViewUsersWithNewMessages(message, thread_num); break;
            case OperationCode::VIEW_USERS_WITH_PRIVATE_CHAT: onViewUsersWithPrivateChat(message, thread_num); break;
            case OperationCode::GET_PRIVATE_CHAT_ID: onGetPrivateChatID(message, thread_num); break;
            case OperationCode::PRIVATE_CHAT_ADD_MESSAGE: onPrivateChatAddMessage(message, thread_num); break;
            case OperationCode::PRIVATE_CHAT_GET_MESSAGES: onPrivateChatGetMessages(message, thread_num); break;
            case OperationCode::PRIVATE_CHAT_EDIT_MESSAGE: onPrivateChatEditMessages(message, thread_num); break;
            case OperationCode::PRIVATE_CHAT_DELETE_MESSAGE: onPrivateChatDeleteMessages(message, thread_num); break;
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

auto Application::onPrivateChatCheckMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            privateChatCheckMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onNewMessagesInPrivateChat(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            newMessagesInPrivateChat(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onViewUsersWithNewMessages(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            viewUsersWithNewMessages(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onViewUsersWithPrivateChat(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            viewUsersWithPrivateChat(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onGetPrivateChatID(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            getPrivateChatID(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onPrivateChatAddMessage(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            privateChatAddMessage(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onPrivateChatGetMessages(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            privateChatGetMessages(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onPrivateChatEditMessages(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            privateChatEditMessages(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::onPrivateChatDeleteMessages(char* message, int thread_num) -> void
{
    auto code_operation{-1};
    getFromBuffer(message, sizeof(int), code_operation);

    auto code = static_cast<OperationCode>(code_operation);
    switch (code)
    {
        case OperationCode::CHECK_SIZE:
        {
            privateChatDeleteMessages(message + 2 * sizeof(int), _server->getMsgFromClientSize(thread_num), thread_num);
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

auto Application::privateChatCheckMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string message_id{message};
    std::string chat_id{message + message_id.size() + 1};

    std::string query_msg = "SELECT m.id, name, surname, user_id, message, creation_date, edited, editing_date, status "
                            "FROM  Users AS u JOIN Messages AS m ON u.id = m.user_id WHERE chat_id = '" +
                            chat_id + "' AND  m.id = '" + message_id + "' AND user_id = '" + _connected_user_id[thread_num] + "'";
    _data_base->query(query_msg.c_str());

    std::string msg{};
    int msg_row_num{0};
    int msg_column_num{0};
    _data_base->getQueryResult(msg, msg_row_num, msg_column_num);

    //auto err_ptr{_data_base->getMySQLError()};
    //std::cout << err_ptr << std::endl;

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

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

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

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;
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

    std::string new_msg{};
    auto new_row_num{0};
    auto new_column_num{0};
    _data_base->getQueryResult(new_msg, new_row_num, new_column_num);

    _server->resizeCashMessageBuffer(thread_num, new_msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(
        _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::NEW_MESSAGES_IN_COMMON_CHAT));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_column_num);
    if (new_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_msg.c_str(), new_msg.size());
}

auto Application::newMessagesInPrivateChat(char* message, size_t message_size, int thread_num) -> void
{
    std::string query_str = " SELECT user_id, count(*) FROM Messages WHERE chat_id IN (SELECT id FROM Chat WHERE first_user_id = '" +
                            _connected_user_id[thread_num] + "' OR second_user_id = '" + _connected_user_id[thread_num] + "') AND user_id != '" +
                            _connected_user_id[thread_num] + "' AND status != 'done'  GROUP BY user_id";
    _data_base->query(query_str.c_str());

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    std::string new_msg{};
    auto new_row_num{0};
    auto new_column_num{0};
    _data_base->getQueryResult(new_msg, new_row_num, new_column_num);

    _server->resizeCashMessageBuffer(thread_num, new_msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(
        _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::NEW_MESSAGES_IN_PRIVATE_CHAT));
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

auto Application::viewUsersWithNewMessages(char* message, size_t message_size, int thread_num) -> void
{
    std::string query_str = "SELECT user_id, count(*), name, surname FROM Messages AS m JOIN Users AS u ON m.user_id = u.id WHERE chat_id IN(SELECT id FROM "
                            "Chat WHERE first_user_id = '" +
                            _connected_user_id[thread_num] + "' OR second_user_id = '" + _connected_user_id[thread_num] + "') AND user_id != '" +
                            _connected_user_id[thread_num] + "' AND status != 'done' GROUP BY user_id";

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

auto Application::viewUsersWithPrivateChat(char* message, size_t message_size, int thread_num) -> void
{
    std::string query_str = "SELECT u.id, name, surname FROM Chat AS c JOIN Users AS u ON c.first_user_id = u.id OR c.second_user_id = u.id WHERE u.id != '" +
                            _connected_user_id[thread_num] + "'";

    _data_base->query(query_str.c_str());

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    std::string users_data{};
    auto data_row_num{0};
    auto data_column_num{0};
    _data_base->getQueryResult(users_data, data_row_num, data_column_num);

    _server->resizeCashMessageBuffer(thread_num, users_data.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(
        _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::VIEW_USERS_WITH_PRIVATE_CHAT));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_column_num);
    if (data_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), users_data.c_str(), users_data.size());
}

auto Application::getPrivateChatID(char* message, size_t message_size, int thread_num) -> void
{
    std::string target_user_id{message};
    std::string query_str = "SELECT id FROM Users WHERE id ='" + target_user_id + "'";
    _data_base->query(query_str.c_str());

    std::string users_data{};
    auto data_row_num{0};
    auto data_column_num{0};
    _data_base->getQueryResult(users_data, data_row_num, data_column_num);

    if (!data_row_num)
    {
        _server->resizeCashMessageBuffer(thread_num, HEADER_SIZE);
        _server->getCashMessageSizeRef(thread_num) = 0;
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::GET_PRIVATE_CHAT_ID));
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_row_num);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_column_num);
        return;
    }

    query_str = "SELECT id FROM Chat WHERE (first_user_id ='" + target_user_id + "' AND second_user_id = '" + _connected_user_id[thread_num] +
                "') OR (first_user_id ='" + _connected_user_id[thread_num] + "' AND second_user_id = '" + target_user_id + "' )";
    _data_base->query(query_str.c_str());

    std::string chat_data{};
    data_row_num = 0;
    data_column_num = 0;
    _data_base->getQueryResult(chat_data, data_row_num, data_column_num);

    if (data_row_num)
    {
        _server->resizeCashMessageBuffer(thread_num, chat_data.size() + HEADER_SIZE);
        _server->getCashMessageSizeRef(thread_num) = 0;
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::GET_PRIVATE_CHAT_ID));
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_row_num);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_column_num);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), chat_data.c_str(), chat_data.size());
        return;
    }

    query_str = "INSERT INTO Chat (first_user_id, second_user_id) VALUES('" + _connected_user_id[thread_num] + "','" + target_user_id + "')";
    _data_base->query(query_str.c_str());

    query_str = "SELECT id FROM Chat WHERE first_user_id = '" + _connected_user_id[thread_num] + "' AND  second_user_id = '" + target_user_id + "'";
    _data_base->query(query_str.c_str());

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    std::string chat_id{};
    data_row_num = 0;
    data_column_num = 0;
    _data_base->getQueryResult(chat_id, data_row_num, data_column_num);

    _server->resizeCashMessageBuffer(thread_num, chat_id.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::GET_PRIVATE_CHAT_ID));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), data_column_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), chat_id.c_str(), chat_id.size());
}

auto Application::privateChatAddMessage(char* message, size_t message_size, int thread_num) -> void
{
    std::string chat_message{message};
    std::string chat_id{message + chat_message.size() + 1};

    std::string query_str = "INSERT INTO Messages (chat_id, user_id, message, creation_date, status)"
                            "VALUES('" +
                            chat_id + "', '" + _connected_user_id[thread_num] + "', '" + message + "', now(), 'delivery')";
    _data_base->query(query_str.c_str());

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;

    const char* result{nullptr};
    if (auto err_ptr{_data_base->getMySQLError()})
    {
        result = RETURN_ERROR.c_str();
    }
    else
    {
        result = RETURN_OK.c_str();
    }
    _server->resizeCashMessageBuffer(thread_num, strlen(result) + HEADER_SIZE);

    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), result, strlen(result));
}

auto Application::privateChatGetMessages(char* message, size_t message_size, int thread_num) -> void
{
    std::string chat_id{message};
    std::string query_str = "SELECT MIN(creation_date) FROM Messages WHERE chat_id ='" + chat_id + "' AND status = 'delivery' AND user_id != '" +
                            _connected_user_id[thread_num] + "'";
    _data_base->query(query_str.c_str());
    std::string query_viewdate{};
    int row_num{0};
    int column_num{0};
    _data_base->getQueryResult(query_viewdate, row_num, column_num);
    std::string lastview{query_viewdate.c_str()};

    if (lastview == "NULL")
    {
        query_str = "SELECT m.id, name, surname, user_id, message, creation_date, edited, editing_date, status "
                    "FROM  Users AS u JOIN Messages AS m ON u.id = m.user_id WHERE chat_id = '" +
                    chat_id + "'";
        _data_base->query(query_str.c_str());

        std::string msg{};
        int row_num{0};
        int column_num{0};
        _data_base->getQueryResult(msg, row_num, column_num);

        _server->resizeCashMessageBuffer(thread_num, msg.size() + HEADER_SIZE);
        _server->getCashMessageSizeRef(thread_num) = 0;
        addToBuffer(
            _server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::PRIVATE_CHAT_GET_MESSAGES));
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), row_num);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), column_num);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), 0);
        addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), 0);

        if (row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), msg.c_str(), msg.size());

        query_str = "UPDATE Messages SET status = 'done' WHERE user_id != '" + _connected_user_id[thread_num] + "' AND chat_id = '" + chat_id + "'";
        _data_base->query(query_str.c_str());

        // auto err_ptr{_data_base->getMySQLError()};
        // std::cout << err_ptr << std::endl;

        return;
    }

    query_str = "SELECT m.id, name, surname, user_id, message, creation_date, edited, editing_date, status "
                "FROM  Users AS u JOIN Messages AS m ON u.id = m.user_id WHERE creation_date < '" +
                lastview + "' AND chat_id = '" + chat_id + "'";
    _data_base->query(query_str.c_str());

    std::string old_msg{};
    int old_row_num{0};
    int old_column_num{0};
    _data_base->getQueryResult(old_msg, old_row_num, old_column_num);
    query_str = "SELECT m.id, name, surname, user_id, message, creation_date, edited, editing_date, status "
                "FROM  Users AS u JOIN Messages AS m ON u.id = m.user_id WHERE creation_date >= '" +
                lastview + "' AND chat_id = '" + chat_id + "'";
    _data_base->query(query_str.c_str());

    std::string new_msg{};
    auto new_row_num{0};
    auto new_column_num{0};
    _data_base->getQueryResult(new_msg, new_row_num, new_column_num);

    _server->resizeCashMessageBuffer(thread_num, old_msg.size() + new_msg.size() + HEADER_SIZE);
    _server->getCashMessageSizeRef(thread_num) = 0;
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), static_cast<int>(OperationCode::PRIVATE_CHAT_GET_MESSAGES));
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_column_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_row_num);
    addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_column_num);
    if (old_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), old_msg.c_str(), old_msg.size());
    if (new_row_num) addToBuffer(_server->getCashMessagePtr(thread_num), _server->getCashMessageSizeRef(thread_num), new_msg.c_str(), new_msg.size());

    query_str = "UPDATE Messages SET status = 'done' WHERE user_id != '" + _connected_user_id[thread_num] + "' AND chat_id = '" + chat_id + "'";
    _data_base->query(query_str.c_str());

    // auto err_ptr{_data_base->getMySQLError()};
    // std::cout << err_ptr << std::endl;
}

auto Application::privateChatEditMessages(char* message, size_t message_size, int thread_num) -> void
{
    std::string msg_id{message};
    std::string chat_id{message + msg_id.size() + 1};
    std::string edited_message{message + chat_id.size() + 1 + msg_id.size() + 1};

    std::string query_msg = "UPDATE Messages SET message = '" + edited_message + "', edited = 1, editing_date = now() WHERE id = '" + msg_id +
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

auto Application::privateChatDeleteMessages(char* message, size_t message_size, int thread_num) -> void
{
    std::string msg_id{message};
    std::string chat_id{message + msg_id.size() + 1};

    std::string query_msg =
        "DELETE FROM Messages WHERE id = '" + msg_id + "' AND user_id = '" + _connected_user_id[thread_num] + "' AND chat_id ='" + chat_id + "'";
    _data_base->query(query_msg.c_str());

    query_msg = "SET @count = 0";
    _data_base->query(query_msg.c_str());

    query_msg = "UPDATE Messages SET Messages.id = @count:= @count + 1";
    _data_base->query(query_msg.c_str());

    query_msg = "ALTER TABLE Messages AUTO_INCREMENT = 1";
    _data_base->query(query_msg.c_str());

     auto err_ptr{_data_base->getMySQLError()};
     std::cout << err_ptr << std::endl;
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
