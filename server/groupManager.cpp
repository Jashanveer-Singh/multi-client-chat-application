#include "groupManager.h"

void GroupManager::addGroup(const std::string& groupName, const std::vector<std::string>& members) {
	std::lock_guard<std::mutex> lock(mtx);
	group[groupName] = members;
}
std::vector<std::string> GroupManager::getGroupMembers(const std::string& groupName) {
	std::lock_guard<std::mutex> lock(mtx);
	if (group.count(groupName)) return group[groupName];
	return {};
}
void GroupManager::removeMember(const std::string& member) {
	std::lock_guard<std::mutex> lock(mtx);
	for (auto& [grp, members] : group) {
		members.erase(std::remove(members.begin(), members.end(), member), members.end());
	}
}
