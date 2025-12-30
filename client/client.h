#pragma once

//#define NOMINMAX
#include <WinSock2.h>
#include <string>
#include <mutex>

#include "message.h"
#include "ui.h"

#define PORT 8080

class Client {
	std::string username;
	std::string recipient;
	std::string group;
	SOCKET sock;
	std::mutex mtx;
	bool running{ false };

public:

	Client() = default;
	Client(const Client&) = delete;
	bool start();
	void stop();
	bool login(const std::string username, const std::string password);
	bool Register(const std::string username, const std::string password);
	void sendMessage(const std::string& msg);
	std::string receiveMessage();
	void setRecipient(std::string recipient);
	void setGroup(std::string group);
	bool isRunning();
	bool createGroup(const std::string& groupName, const std::vector<std::string>& members);
};