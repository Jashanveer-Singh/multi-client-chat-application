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
	std::string groupFile = "groups.txt";
    std::mutex dbMutex;

public:
    bool userExists(const std::string& username) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ifstream in(userFile);
        std::string u, p;
        while (in >> u >> p) {
            if (u == username) return true;
        }
        return false;
    }

    bool validateUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ifstream in(userFile);
        std::string u, p;
        while (in >> u >> p) {
            if (u == username && p == password) return true;
        }
        return false;
    }

    void addUser(const std::string& username, const std::string& password) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ofstream out(userFile, std::ios::app);
        out << username << " " << password << "\n";
    }

    void saveMessage(const std::string& from, const std::string& to, const std::string& msg, const std::string& group) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ofstream out(msgFile, std::ios::app);
        out << from << "|" << to << "|" << group << "|" << msg << "\n";
    }

    void createGroup(const std::string& groupName, const std::vector<std::string>& members) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ofstream out(groupFile, std::ios::app);
        out << groupName;
        for (const auto& member : members) {
            out << "|" << member;
        }
        out << "\n";
	}

    void getGroupMembers(const std::string& groupName, std::vector<std::string>& members) {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::ifstream in(groupFile);
        std::string line;
        while (std::getline(in, line)) {
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                std::string gName = line.substr(0, pos);
                if (gName == groupName) {
                    size_t start = pos + 1;
                    while ((pos = line.find('|', start)) != std::string::npos) {
                        members.push_back(line.substr(start, pos - start));
                        start = pos + 1;
                    }
                    members.push_back(line.substr(start));
                    return;
                }
            }
        }
    }
};