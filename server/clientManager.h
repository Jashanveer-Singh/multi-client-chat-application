#pragma once
#include <map>
#include <string>
#include <winsock2.h>
#include <mutex>

class ClientManager {
    std::map<std::string, SOCKET> activeClients;
    std::mutex mtx;

public:
    void addClient(const std::string& username, SOCKET s) {
        std::lock_guard<std::mutex> lock(mtx);
        activeClients[username] = s;
    }

    void removeClient(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);
        activeClients.erase(username);
    }

    SOCKET getSocket(const std::string& username) {
        std::lock_guard<std::mutex> lock(mtx);
        if (activeClients.count(username)) return activeClients[username];
        return INVALID_SOCKET;
    }

    std::map<std::string, SOCKET> getAllClients() {
        std::lock_guard<std::mutex> lock(mtx);
        return activeClients;
    }
};