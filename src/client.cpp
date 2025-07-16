#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main(){

WSADATA wsaData;

int result = WSAStartup(MAKEWORD(2,2), &wsaData);

if (result != 0){
    cerr<<"WSAStartup failed with error : "<< result<<endl;
    return 1;
}

SOCKET ssocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

if(ssocket == INVALID_SOCKET){
    cerr << "Socket creation failed: " << WSAGetLastError() << endl;
    WSACleanup();
    return 1;
}

cout << "TCP socket created successfully!" << endl;

sockaddr_in server_addr;

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(9000);
server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


if (connect(ssocket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    cerr << "Connect failed: " << WSAGetLastError() << endl;
    closesocket(ssocket);
    WSACleanup();
    return 1;
}

cout << "Successfully connected to server!" << endl;

while(true){
    string cmd;
    cout<<"Enter the command: ";
    getline(cin, cmd);
    if(cmd == "exit"){
        break;
    }
    cmd += "\n";

    int bytes_sent = send(ssocket, cmd.c_str(), cmd.length(), 0);

    if(bytes_sent == SOCKET_ERROR){
         cerr << "Send failed with error: " << WSAGetLastError() << endl;

    }

// Inside main while loop, after send()

char buffer[4096];
string response_buffer;

// Loop until we get a newline from the server
while (response_buffer.find('\n') == string::npos) {
    int bytes_received = recv(ssocket, buffer, sizeof(buffer) - 1, 0); // -1 for null terminator
    
    if (bytes_received <= 0) {
        cout << "\nServer disconnected." << endl;
        closesocket(ssocket); // Close socket immediately
        WSACleanup();
        return 1; // Exit program
    }

    // Null-terminate the received data and append to our string buffer
    buffer[bytes_received] = '\0';
    response_buffer.append(buffer);
}

cout << "Server: " << response_buffer.substr(0, response_buffer.length() - 1) << endl;
}

WSACleanup();
return 0;
}