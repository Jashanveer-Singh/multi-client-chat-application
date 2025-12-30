#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>

struct UserRecord {
    std::string username;
    std::string password;
};

class Repository {
    std::string userFile = "users.txt";
    std::string msgFile = "chat_history.txt";
    std::mutex dbMutex;

public:
    bool userExists(const std::string& username);

    bool validateUser(const std::string& username, const std::string& password);

    void addUser(const std::string& username, const std::string& password);

    void saveMessage(const std::string& from, const std::string& to, const std::string& msg, const std::string& group);

    
};