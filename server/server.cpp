#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include "message.h"
#include "repo.h"
#include "ClientManager.h"

#pragma comment(lib, "ws2_32.lib")

class ChatServer {
    SOCKET listenSocket;
    Repository repo;
    ClientManager cm;
    const int MAX_CLIENTS = 50;
    int currentClients = 0;
    std::mutex countMtx;

public:
    ChatServer(int port) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        listenSocket = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        listen(listenSocket, SOMAXCONN);
    }

    void run() {
        std::cout << "Server started on port 8080..." << std::endl;
        while (true) {
            sockaddr_in clientAddr;
            int clientSize = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientSize);

            std::lock_guard<std::mutex> lock(countMtx);
            if (currentClients >= MAX_CLIENTS) {
                closesocket(clientSocket);
                continue;
            }
            currentClients++;
            std::thread(&ChatServer::handleClient, this, clientSocket).detach();
        }
    }

private:
    void handleClient(SOCKET s) {
        std::string currentUser = "";
		char headerBuf[2];
		message::msgHeader header;

        while (true) {
            int res = recv(s, headerBuf, 2, 0);
            if (res <= 0) break; // Abrupt disconnection

			header.fromBytes(headerBuf);

            std::vector<char> body(header.getMsgSize());
            recv(s, body.data(), header.getMsgSize(), 0);

            switch (header.getMsgType())
            {
            case (uint8_t)message::CLIENTMSGTYPE::INVALID:
                continue; // Ignore invalid messages

            case (uint8_t)message::CLIENTMSGTYPE::LOGIN:
            {
                message::client::Login logMsg;
                logMsg.fromBytes(body.data());
                //if (repo.validateUser(logMsg.getUsername(), logMsg.getPassword())) {
                    currentUser = logMsg.getUsername();
                    cm.addClient(currentUser, s);
                //}

                break;
            }

            case (uint8_t)message::CLIENTMSGTYPE::REGISTER:
            {
                message::client::Register regMsg;
                regMsg.fromBytes(body.data());
                if (repo.userExists(regMsg.getUsername())) {
                    // Handle user already exists (send failure response)
                    break;
                }
                repo.addUser(regMsg.getUsername(), regMsg.getPassword());

                break;
            }

            case (uint8_t)message::CLIENTMSGTYPE::MESSAGE:
            {
                message::client::Message msg;
                msg.fromBytes(body.data());
                if (!currentUser.empty()) {
                    repo.saveMessage(currentUser, msg.getRecipient(), msg.getMsg(), msg.getGroup());

					std::string sendBuffer;
					message::server::Message serverMsg(currentUser, msg.getGroup(), msg.getMsg());
                    message::msgHeader serverHeader((uint8_t)message::SERVERMSGTYPE::MESSAGE, serverMsg.getMsgSize());

					serverHeader.toBytes(sendBuffer);
					serverMsg.toBytes(sendBuffer);

                    if (!msg.getGroup().empty()) {
                        sendToGroup(msg.getGroup(), sendBuffer);
                    } else {
                        sendToClient(msg.getRecipient(), sendBuffer);
                    }
				}
                break;
            }

            case (uint8_t)message::CLIENTMSGTYPE::KEEPALIVE:
            {

            break;
            }

            default:
                break;
            }
        }

        // Cleanup on disconnection
        if (!currentUser.empty()) cm.removeClient(currentUser);
        closesocket(s);
        std::lock_guard<std::mutex> lock(countMtx);
        currentClients--;
        std::cout << "Client disconnected. Total: " << currentClients << std::endl;
    }

    void sendToClient(const std::string& username, const std::string& msg) {
        SOCKET s = cm.getSocket(username);
        if (s != INVALID_SOCKET) {
            send(s, msg.c_str(), msg.size(), 0);
        }
	}

    void sendToGroup(const std::string& groupName, const std::string& msg) {
        std::vector<std::string> members;
        repo.getGroupMembers(groupName, members);
        for (const auto& member : members) {
            sendToClient(member, msg);
        }
	}
};

int main() {
    ChatServer server(8080);
    server.run();
    return 0;
}