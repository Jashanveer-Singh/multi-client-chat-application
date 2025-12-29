#pragma once

#include <map>
#include <string>
#include <winsock2.h>
#include <mutex>
#include <vector>
class GroupManager {
	std::map<std::string, std::vector<std::string>> group;
	std::mutex mtx;
public:
	void addGroup(const std::string& groupName, const std::vector<std::string>& members);
	std::vector<std::string> getGroupMembers(const std::string& groupName);
	void removeMember(const std::string& member);
};
