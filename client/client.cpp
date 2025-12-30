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

	return true;
}

void Client::stop() {
	running = false;
	closesocket(sock);
	WSACleanup();
}

std::string Client::receiveMessage() {
	char headerBuffer[2];
	int res = recv(sock, headerBuffer, sizeof(headerBuffer), 0);
	if (res <= 0) {
		std::cerr << "Receive failed or connection closed\n";
		stop();
		return "";
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
			return "";
		}
		serverMsg.fromBytes(msgBuffer.c_str());
		std::string fullMsg;
		fullMsg += serverMsg.getSender();
		if (!serverMsg.getGroup().empty()) {
			fullMsg += " [" + serverMsg.getGroup() + "]: ";
		}
		fullMsg += serverMsg.getMsg();
		return fullMsg;
	}


	break;
	}
	return std::string{};
}

void Client::sendMessage(const std::string& msg) {

	message::client::Message clientMsg(recipient, group, msg);
	message::msgHeader header{ (uint8_t)message::CLIENTMSGTYPE::MESSAGE, clientMsg.getMsgSize() };
	std::string msgBuffer;
	header.toBytes(msgBuffer);
	clientMsg.toBytes(msgBuffer);
	send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);

}

bool Client::login(const std::string username, const std::string password) {
	this->username = username;
	message::client::Login loginMsg(username, password);
	message::msgHeader header{ (uint8_t)message::CLIENTMSGTYPE::LOGIN, loginMsg.getMsgSize() };
	std::string msgBuffer;
	header.toBytes(msgBuffer);
	loginMsg.toBytes(msgBuffer);
	send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);

	int res = recv(sock, msgBuffer.data(), 2, 0);
	if (res <= 0) {
		std::cerr << "Receive failed or connection closed\n";
		stop();
		return false;
	}
	header.fromBytes(msgBuffer.c_str());

	if (header.getMsgType() == (uint8_t)message::SERVERMSGTYPE::OK) {
		this->username = username;
		return true;
	}
	return false;

}

bool Client::Register(const std::string username, const std::string password) {
	message::client::Auth registerMsg(message::CLIENTMSGTYPE::REGISTER, username, password);
	message::msgHeader header{ (uint8_t)message::CLIENTMSGTYPE::REGISTER, registerMsg.getMsgSize() };
	std::string msgBuffer;
	header.toBytes(msgBuffer);
	registerMsg.toBytes(msgBuffer);
	send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);

	int res = recv(sock, msgBuffer.data(), 2, 0);
	if (res <= 0) {
		std::cerr << "Receive failed or connection closed\n";
		stop();
		return false;
	}
	header.fromBytes(msgBuffer.c_str());

	if (header.getMsgType() == (uint8_t)message::SERVERMSGTYPE::OK) {
		this->username = username;
		return true;
	}
	return false;
}

void Client::setRecipient(std::string recipient) {
	this->recipient = recipient;
	group.clear();
}

bool Client::isRunning() {
	return running;
}

void Client::setGroup(std::string group) {
	this->group = group;
	recipient.clear();
}

bool Client::createGroup(const std::string& groupName, const std::vector<std::string>& members) {
	message::client::CreateGroup grpMsg(groupName, members);
	message::msgHeader header{ (uint8_t)message::CLIENTMSGTYPE::CREATE_GROUP, grpMsg.getMsgSize() };
	std::string msgBuffer;
	header.toBytes(msgBuffer);
	grpMsg.toBytes(msgBuffer);
	send(sock, msgBuffer.c_str(), msgBuffer.size(), 0);
	int res = recv(sock, msgBuffer.data(), 2, 0);
	if (res <= 0) {
		std::cerr << "Receive failed or connection closed\n";
		stop();
		return false;
	}
	header.fromBytes(msgBuffer.c_str());
	if (header.getMsgType() == (uint8_t)message::SERVERMSGTYPE::OK) {
		this->group = groupName;
		return true;
	}
	return false;
}