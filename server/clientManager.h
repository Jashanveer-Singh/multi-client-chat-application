#pragma once
#include <map>
#include <string>
#include <winsock2.h>
#include <mutex>

class ClientManager {
    std::map<std::string, SOCKET> activeClients;
    std::mutex mtx;

public:
    void addClient(const std::string& username, SOCKET s);

    void removeClient(const std::string& username);

    SOCKET getSocket(const std::string& username);

    std::map<std::string, SOCKET> getAllClients();
    }
};