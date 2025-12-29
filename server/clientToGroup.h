#pragma once

#include <string>
#include <vector>
#include <map>
#include <winsock2.h>

class ClientToGroup {
	std::map<std::string, std::vector<std::string>> clientToGroup;
public:
	void addGroupToClient(const std::string& client, const std::string& group);
	std::vector<std::string> getGroupsOfClient(const std::string& client);
	void removeClient(const std::string& client);
};