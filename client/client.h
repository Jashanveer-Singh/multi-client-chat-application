#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <mutex>

#include "message.h"

#define PORT 8080

class Client {
	std::string username;
	std::string recipient;
	std::string password;
	SOCKET sock;
	std::mutex mtx;
	bool running{ false };

public:

	bool start();
	void stop();
	void login();
	void Register();
	void menu();
	void sendMessage(const std::string& msg);
	void receiveMessage();
	void setRecipient();
};