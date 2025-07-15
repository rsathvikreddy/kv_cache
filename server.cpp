#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "basic.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void handle_client(SOCKET client_socket, sockaddr_in client_addr, unordered_map<string,string>& kv_store) {
 
    // Show client IP and print the connection ip and port
    cout << "Client connected from " 
         << inet_ntoa(client_addr.sin_addr) 
         << ":" << ntohs(client_addr.sin_port) << endl;
    
    char buffer[4096];
    string cmd_buffer;

    while(true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if(bytes_received <= 0) {
            // print dissconnection with client IP and port
            cout << "Client disconnected from " 
                 << inet_ntoa(client_addr.sin_addr) 
                 << ":" << ntohs(client_addr.sin_port) << endl;
            break;
        }
        
        cmd_buffer.append(buffer, bytes_received);
        size_t pos;
        while ((pos = cmd_buffer.find('\n')) != string::npos) {
            string command = cmd_buffer.substr(0, pos);
            cmd_buffer.erase(0, pos + 1);
            
            cout << "Received command: " << command << endl;
            
            istringstream iss(command);
            vector<string> tokens;
            string word;
            while (iss >> word) {
                tokens.push_back(word);
            }

            string ans;
            if(!tokens.empty()) {
                if(tokens[0] == "put") {
                    ans = put(tokens, kv_store);
                }
                else if(tokens[0] == "get") {
                    ans = get(tokens, kv_store);
                }
                else if(tokens[0] == "delete") {
                    ans = deletee(tokens, kv_store);
                }
                else {
                    ans = "ERROR: Invalid command";
                }
            }
            
            ans += "\n";
            if(send(client_socket, ans.c_str(), ans.length(), 0) == SOCKET_ERROR) {
                cerr << "Send failed: " << WSAGetLastError() << endl;
                break;
            }
        }
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
        cerr <<"WSAStartup failed with error : "<< result<< endl;
        return 1;
    }

    SOCKET ssocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ssocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    cout << "TCP socket created successfully!" << endl;

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9000);

    if(bind(ssocket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(ssocket);
        WSACleanup();
        return 1;
    }

    cout << "Successfully bound to port 9000!" << endl;

    if(listen(ssocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(ssocket);
        WSACleanup();
        return 1;
    }

    cout << "Server running on port 9000..." << endl;

    unordered_map<string, string> kv_store;
    vector<thread> threads;

    while(true) {
        // 1. Accept client
        sockaddr_in client_addr{};

        int client_addr_size = sizeof(client_addr);
        SOCKET client_socket = accept(ssocket, (sockaddr*)&client_addr, &client_addr_size);
        
        if(client_socket == INVALID_SOCKET) {
            cerr << "Accept failed: " << WSAGetLastError() << endl;
            continue;
        }


        threads.emplace_back(handle_client, client_socket, client_addr, std::ref(kv_store));
        threads.back().detach();  // Detach to run independently
    }

    closesocket(ssocket);
    WSACleanup();
    return 0;
}

//g++ server.cpp basic.cpp -o server -lws2_32
//telnet 127.0.0.1:9000