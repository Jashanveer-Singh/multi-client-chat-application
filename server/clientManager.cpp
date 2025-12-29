#include "clientManager.h"

void ClientManager::addClient(const std::string& username, SOCKET s) {
	std::lock_guard<std::mutex> lock(mtx);
	activeClients[username] = s;
}

void ClientManager::removeClient(const std::string& username) {
	std::lock_guard<std::mutex> lock(mtx);
	activeClients.erase(username);
}

SOCKET ClientManager::getSocket(const std::string& username) {
	std::lock_guard<std::mutex> lock(mtx);
	if (activeClients.count(username)) return activeClients[username];
	return INVALID_SOCKET;
}

std::map<std::string, SOCKET> ClientManager::getAllClients() {
	std::lock_guard<std::mutex> lock(mtx);
	return activeClients;
}