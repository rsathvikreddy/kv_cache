#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "kvstore.h"


// The handler now accepts a reference to the KVStore object
void handle_client(SOCKET client_socket, sockaddr_in client_addr, KVStore& store) {
    std::cout << "Client connected from "
         << inet_ntoa(client_addr.sin_addr)
         << ":" << ntohs(client_addr.sin_port) << std::endl;

    char buffer[4096];
    std::string cmd_buffer;

    while(true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if(bytes_received <= 0) {
            std::cout << "Client disconnected from "
                 << inet_ntoa(client_addr.sin_addr)
                 << ":" << ntohs(client_addr.sin_port) << std::endl;
            break;
        }

        cmd_buffer.append(buffer, bytes_received);
        size_t pos;
        while ((pos = cmd_buffer.find('\n')) != std::string::npos) {
            std::string command = cmd_buffer.substr(0, pos);
            cmd_buffer.erase(0, pos + 1);

            std::cout << "Received command: " << command << std::endl;
            
            // Process the command using the store object
            std::string ans = store.process_command(command);

            ans += "\n";
            if(send(client_socket, ans.c_str(), ans.length(), 0) == SOCKET_ERROR) {
                std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
                break;
            }
        }
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listen_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9000);

    if(bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    if(listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Server running on port 9000..." << std::endl;

    KVStore store;      // Create ONE instance of the key-value store.
    store.load_from_file(); // Load the data.

    while(true) {
        sockaddr_in client_addr{};
        int client_addr_size = sizeof(client_addr);
        SOCKET client_socket = accept(listen_socket, (sockaddr*)&client_addr, &client_addr_size);

        if(client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // Pass a reference to the single 'store' object to each new thread.
        std::thread client_thread(handle_client, client_socket, client_addr, std::ref(store));
        client_thread.detach();
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}

//g++ server.cpp basic.cpp -o server -lws2_32