///////////////////DEBUG/////////////////////////
#include <iostream>
///////////////////DEBUG/////////////////////////

#include "Server.h"
#include "Application.h"
#include "core.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
//#pragma comment(lib, "AdvApi32.lib")

const char* DEFAULT_PORT = "27777";

#elif defined __linux__
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int DEFAULT_PORT = 27777;  // A
#endif  //  _WIN32

#ifdef _WIN32
auto Server::server_thread(int thread_number) -> void
{
    // TODO mutex
    _cash_message.push_back(nullptr);        ////////////////////////////////////////////////////////////
    _cash_message_size.push_back(0);         ///////////////////////////////////////////////////////////
    _cash_message_buffer_size.push_back(0);  ///////////////////////////////////////////////////
    _exchange_buffer_size.push_back(DEFAULT_BUFLEN);
    _need_buffer_resize.push_back(true);
    _exchange_message.push_back(nullptr);  ///////////////////////////////////////////////////////
    _exchange_message_size.push_back(0);         ////////////////////////////////////////////////////////
    _msg_from_client_size.push_back(0);    /////////////////////////////////////////////////////////////
    //_in_message_ready.push_back(false);
    //_out_message_ready.push_back(false);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    _clients.emplace_back(&Server::server_thread, this, thread_count);
    ++thread_count;
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    _clients.back().detach();

    // Receive until the peer shuts down the connection

    size_t current_buffer_size{0};

    do
    {
        if (_need_buffer_resize[thread_number])
        {
            if (current_buffer_size < _exchange_buffer_size[thread_number])
            {
                current_buffer_size = _exchange_buffer_size[thread_number];
                _exchange_message[thread_number] = std::shared_ptr<char[]>(new char[current_buffer_size]);
            }
            _need_buffer_resize[thread_number] = false;
        }

        iResult = recv(ClientSocket, _exchange_message[thread_number].get(), current_buffer_size, 0);
        if (iResult > 0)
        {
            //_in_message_ready[thread_number] = true;

            _app->reaction(_exchange_message[thread_number].get(), thread_number);  //

            if (*(reinterpret_cast<int*>(_exchange_message[thread_number].get())) == static_cast<int>(OperationCode::STOP))
            {
                std::cout << "Client Exited." << std::endl;
                break;
            }

            //while (!_out_message_ready[thread_number])
            //{
            //}

            //std::this_thread::sleep_for(std::chrono::milliseconds(1));  // NEED

            //std::cout << "Send start!" << std::endl;

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, _exchange_message[thread_number].get(), _exchange_message_size[thread_number], 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }

            //_out_message_ready[thread_number] = false;
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

//    _exchange_message[thread_number] = nullptr;
//    _cash_message[thread_number] = nullptr;

    return;
}

#elif defined __linux__
auto Server::client_loop(int thread_number, int connection) -> void
{
    _cash_message.push_back(nullptr);        
    _cash_message_size.push_back(0);         
    _cash_message_buffer_size.push_back(0);  
    _exchange_buffer_size.push_back(DEFAULT_BUFLEN);
    _need_buffer_resize.push_back(true);
    _exchange_message.push_back(nullptr);  
    _exchange_message_size.push_back(0);         
    _msg_from_client_size.push_back(0);   
    //_in_message_ready.push_back(false);
    //_out_message_ready.push_back(false);

    size_t current_buffer_size{0};

    while (1)
    {
        if (_need_buffer_resize[thread_number])
        {
            if (current_buffer_size < _exchange_buffer_size[thread_number])
            {
                current_buffer_size = _exchange_buffer_size[thread_number];
                _exchange_message[thread_number] = std::shared_ptr<char[]>(new char[current_buffer_size]);
            }
            _need_buffer_resize[thread_number] = false;
        }

        ssize_t length = read(connection, _exchange_message[thread_number].get(), current_buffer_size);

        // _in_message_ready[thread_number] = true;

        _app->reaction(_exchange_message[thread_number].get(), thread_number);  //

        if (*(reinterpret_cast<int*>(_exchange_message[thread_number].get())) == static_cast<int>(OperationCode::STOP))
        {
            std::cout << "Client Exited." << std::endl;
            break;
        }

        //while (!_out_message_ready[thread_number])
        //{
        //}

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));  // NEED
        
        ssize_t bytes = write(connection, _exchange_message[thread_number].get(), _exchange_message_size[thread_number]);
        //if (bytes >= 0)
        //{
        //    _out_message_ready[thread_number] = false;
        //}
    }
    // close socket
    close(connection);

//   _exchange_message[thread_number] = nullptr;
//    _cash_message[thread_number] = nullptr;

    return;
}
auto Server::server_thread() -> int
{
    struct sockaddr_in serveraddress, client;
    socklen_t length;
    int sockert_file_descriptor, connection, bind_status, connection_status;

    sockert_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (sockert_file_descriptor == -1)
    {
        std::cout << "Socket creation failed.!" << std::endl;
        exit(1);
    }
    //
    serveraddress.sin_addr.s_addr = INADDR_ANY;
    serveraddress.sin_port = htons(DEFAULT_PORT);
    serveraddress.sin_family = AF_INET;
    bind_status = bind(sockert_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if (bind_status == -1)
    {
        std::cout << "Socket binding failed.!" << std::endl;
        exit(1);
    }
    connection_status = listen(sockert_file_descriptor, 5);
    if (connection_status == -1)
    {
        std::cout << "Socket is unable to listen for new connections.!" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Server is listening for new connection: " << std::endl;
    }
    while (1)
    {
        length = sizeof(client);
        connection = accept(sockert_file_descriptor, (struct sockaddr*)&client, &length);
        if (connection == -1)
        {
            std::cout << "Server is unable to accept the data from client.!" << std::endl;
            exit(1);
        }
        _clients.emplace_back(&Server::client_loop, this, thread_count, connection);
        ++thread_count;
        _clients.back().detach();
    }
    close(sockert_file_descriptor);

    return 0;
}

#endif  // _WIN32

//auto Server::main_loop(Application* app) -> void
//{
//    while (continue_flag)
//    {
//        for (auto i = 0; i < _out_message_ready.size(); ++i)
//        {
//
//            if (!_in_message_ready[i]) continue;
//
//            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
//
//            // std::cout << "In message: " << in_message[i] << " " << i << std::endl;
//
//            app->reaction(_exchange_message[i].get(), i);  //
//
//            std::cout << "Reaction end!" << std::endl;
//
//            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
//
//            _in_message_ready[i] = false;
//            _out_message_ready[i] = true;
//        }
//        // std::cin >> msg;
//        // if (msg == "end") break;
//    }
//}

Server::Server(Application* app) : _app(app)
{
}
Server::~Server()
{
}

auto Server::run() -> void
{
    std::string msg{};
#ifdef _WIN32
    _clients.emplace_back(&Server::server_thread, this, thread_count);
    ++thread_count;
    _clients.back().detach();
#elif defined __linux__
    std::thread t(&Server::server_thread, this);
    t.detach();
#endif  // _WIN32

    //std::thread t1(&Server::main_loop, this, _app);
    //t1.detach();

    return;
}

auto Server::setContinueFlag(bool flag) -> void
{
    continue_flag = flag;
}

auto Server::setBufferSize(int index, size_t size) -> void
{
    _exchange_buffer_size[index] = size;
    _need_buffer_resize[index] = true;
}

auto Server::resizeCashMessageBuffer(int thread_num, size_t new_size) -> void
{
    if (new_size > _cash_message_buffer_size[thread_num])
    {
        // delete[] _cash_message[thread_num];
        _cash_message[thread_num] = std::shared_ptr<char[]>(new char[new_size]);
        _cash_message_buffer_size[thread_num] = new_size;
//        std::cout << "Resize cash mesage to: " << new_size << std::endl;
    }
}

// auto Server::setCashMessage(const std::string& msg, int thread_num) -> void
//{
//    cash_message[thread_num] = msg;
//}
