#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include "message.h"
#include "repo.h"
#include "ClientManager.h"
#include "groupManager.h"
#include "clientToGroup.h"

#pragma comment(lib, "ws2_32.lib")

class ChatServer {
    SOCKET listenSocket;
    Repository repo;
    ClientManager cm;
    GroupManager gm;
	ClientToGroup ctg;
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
    void handleClient(SOCKET sock) {
        std::string currentUser = "";
		char headerBuf[2];
		message::msgHeader header;

        while (true) {
            int res = recv(sock, headerBuf, 2, 0);
            if (res <= 0) break; // Abrupt disconnection

			header.fromBytes(headerBuf);

            std::vector<char> body(header.getMsgSize());
            recv(sock, body.data(), header.getMsgSize(), 0);

            switch (header.getMsgType())
            {
            case (uint8_t)message::CLIENTMSGTYPE::INVALID:
                continue; // Ignore invalid messages

            case (uint8_t)message::CLIENTMSGTYPE::LOGIN:
            {
                message::client::Login logMsg;
                logMsg.fromBytes(body.data());
                if (repo.validateUser(logMsg.getUsername(), logMsg.getPassword())) {
                    currentUser = logMsg.getUsername();
                    cm.addClient(currentUser, sock);

                    std::string serverMsg;
                    message::server::OkResponse okMsg;
                    message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::OK, okMsg.getMsgSize());
                    resHeader.toBytes(serverMsg);
                    okMsg.toBytes(serverMsg);
                    sendToClient(sock, serverMsg);
                }
                else {
                    std::string serverMsg;
                    message::server::FailureResponse failMsg;
                    message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::FAILURE, failMsg.getMsgSize());
                    resHeader.toBytes(serverMsg);
                    failMsg.toBytes(serverMsg);
                    sendToClient(sock, serverMsg);
                }

                break;
            }

            case (uint8_t)message::CLIENTMSGTYPE::REGISTER:
            {
                message::client::Register regMsg;
                regMsg.fromBytes(body.data());
                if (repo.userExists(regMsg.getUsername())) {
                    // Handle user already exists (send failure response)
                    std::string serverMsg;
                    message::server::FailureResponse failMsg;
                    message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::FAILURE, failMsg.getMsgSize());
                    resHeader.toBytes(serverMsg);
                    failMsg.toBytes(serverMsg);
                    sendToClient(sock, serverMsg);
                    break;
                }
                repo.addUser(regMsg.getUsername(), regMsg.getPassword());
				currentUser = regMsg.getUsername();
				cm.addClient(currentUser, sock);

                std::string serverMsg;
                message::server::OkResponse okMsg;
                message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::OK, okMsg.getMsgSize());
                resHeader.toBytes(serverMsg);
                okMsg.toBytes(serverMsg);
                sendToClient(sock, serverMsg);

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
                        sendToGroup(currentUser, msg.getGroup(), sendBuffer);
                    } else {
                        SOCKET sock = cm.getSocket(msg.getRecipient());
                        std::cout << msg.getRecipient() << ": " << sock;
                        if (sock == INVALID_SOCKET) std::cout << " INVALID";
						std::cout << "group: " << msg.getGroup();
                        std::cout << std::endl;
                        sendToClient(cm.getSocket(msg.getRecipient()), sendBuffer);
                    }
				}
                break;
            }

            case (uint8_t)message::CLIENTMSGTYPE::CREATE_GROUP:
            {
                message::client::CreateGroup grpMsg;
                grpMsg.fromBytes(body.data());
                std::vector<std::string> filteredMembers;
                for (const auto& member : grpMsg.getMembers()) {
                    if (cm.userActive(member)) {
                        filteredMembers.push_back(member);
                    }
				}
				filteredMembers.push_back(currentUser);
				gm.addGroup(grpMsg.getGroupName(), filteredMembers);
				for (const auto& member : filteredMembers) {
					ctg.addGroupToClient(member, grpMsg.getGroupName());
				}
                std::string serverMsg;
				if (filteredMembers.empty()) {
                    message::server::FailureResponse failMsg;
                    message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::FAILURE, failMsg.getMsgSize());
                    resHeader.toBytes(serverMsg);
                    failMsg.toBytes(serverMsg);
                    sendToClient(sock, serverMsg);
                    break;
                }
                message::server::OkResponse okMsg;
                message::msgHeader resHeader((uint8_t)message::SERVERMSGTYPE::OK, okMsg.getMsgSize());
                resHeader.toBytes(serverMsg);
                okMsg.toBytes(serverMsg);
                sendToClient(sock, serverMsg);
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
        if (!currentUser.empty()) {
            cm.removeClient(currentUser);
			ctg.removeClient(currentUser);
			gm.removeMember(currentUser);
        }
        closesocket(sock);
        std::lock_guard<std::mutex> lock(countMtx);
        currentClients--;
        std::cout << "Client disconnected. Total: " << currentClients << std::endl;
    }

    void sendToClient(SOCKET sock, const std::string& msg) {
        if (sock != INVALID_SOCKET) {
            send(sock, msg.c_str(), msg.size(), 0);
        }
	}

    void sendToGroup(const std::string& sender, const std::string& groupName, const std::string& msg) {
		auto members = gm.getGroupMembers(groupName);
        for (const auto& member : members) {
			if (member == sender) continue; // Don't send to sender
            sendToClient(cm.getSocket(member), msg);
        }
	}
};

int main() {
    ChatServer server(8080);
    server.run();
    return 0;
}