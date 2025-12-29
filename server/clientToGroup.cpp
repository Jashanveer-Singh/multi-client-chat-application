#include "clientToGroup.h"


void ClientToGroup::addGroupToClient(const std::string& client, const std::string& group) {
	clientToGroup[client].push_back(group);
}
std::vector<std::string> ClientToGroup::getGroupsOfClient(const std::string& client) {
	if (clientToGroup.count(client)) {
		return clientToGroup[client];
	}
	return {};
}
void ClientToGroup::removeClient(const std::string& client) {
	clientToGroup.erase(client);
}
