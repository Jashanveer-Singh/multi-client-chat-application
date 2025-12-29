#include "client.h"
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

bool Client::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return false;
    }

    running = true;
    std::cout << "Connected to chat server!\n";
    
	while (username.empty()) {
        menu();
    }

    std::thread(&Client::receiveMessage, this).detach();
    return true;

}

void Client::stop() {
    running = false;
    closesocket(sock);
    WSACleanup();
}

void Client::receiveMessage() {
    while (running) {
        char headerBuffer[2];
		int res = recv(sock, headerBuffer, sizeof(headerBuffer), 0);
		if (res <= 0) {
            std::cerr << "Receive failed or connection closed\n";
            stop();
            return;
        }
		message::msgHeader header;
		header.fromBytes(headerBuffer);
        switch (header.getMsgType()) {
        case (uint8_t)message::SERVERMSGTYPE::MESSAGE:
            {
                message::server::Message serverMsg;
                std::string msgBuffer;
                msgBuffer.resize(header.getMsgSize());
                int bytesReceived = recv(sock, &msgBuffer[0], header.getMsgSize(), 0);
                if (bytesReceived <= 0) {
                    std::cerr << "Receive failed or connection closed\n";
                    stop();
                    return;
                }
                serverMsg.fromBytes(msgBuffer.c_str());
                std::string msg = serverMsg.getMsg();
                std::cout << "Received: " << msg << "\n";
            }


            break;

            continue;
        default:
            std::cerr << "Unknown message type\n";
            continue;
        }
    }
}

void Client::sendMessage(const std::string& msg) {
	message::client::Message clientMsg(recipient, "", msg);
    message::msgHeader header{(uint8_t)message::CLIENTMSGTYPE::MESSAGE, clientMsg.getMsgSize()};
    std::string msgBuffer;
    header.toBytes(msgBuffer);
	clientMsg.toBytes(msgBuffer);
    send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);
}

void Client::login() {
    std::cout << "Enter username: ";
    std::cin >> username;
	std::cout << "Enter password: ";
	std::cin >> password;
    message::client::Login loginMsg(username, password); 
    message::msgHeader header{(uint8_t)message::CLIENTMSGTYPE::LOGIN, loginMsg.getMsgSize()};
    std::string msgBuffer;
    header.toBytes(msgBuffer);
    loginMsg.toBytes(msgBuffer);
    send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);
}

void Client::Register() {
    std::cout << "Enter username: ";
    std::cin >> username;
	std::cout << "Enter password: ";
	std::cin >> password;
    message::client::Auth registerMsg(message::CLIENTMSGTYPE::REGISTER, username, password); 
    message::msgHeader header{(uint8_t)message::CLIENTMSGTYPE::REGISTER, registerMsg.getMsgSize()};
    std::string msgBuffer;
    header.toBytes(msgBuffer);
    registerMsg.toBytes(msgBuffer);
    send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);
}

void Client::menu() {
    std::cout << "1. Login\n2. Register\nChoose an option: ";
    int choice;
    std::cin >> choice;
    if (choice == 1) {
        login();
    } else if (choice == 2) {
        Register();
    } else {
        std::cout << "Invalid choice\n";
    }
}

void Client::setRecipient() {
    std::cout << "Enter recipient username: ";
    std::cin >> recipient;
}