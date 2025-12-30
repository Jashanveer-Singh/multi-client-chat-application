#include "repo.h"

Repository::Repository() {
	// Ensure user and message files exist
	{
		std::lock_guard<std::mutex> lock(dbMutex);
		std::ofstream userOut(userFile, std::ios::app);
		std::ofstream msgOut(msgFile, std::ios::app);
	}
}

bool Repository::userExists(const std::string& username) {
	std::lock_guard<std::mutex> lock(dbMutex);
	std::ifstream in(userFile);
	std::string u, p;
	while (in >> u >> p) {
		if (u == username) return true;
	}
	return false;
}

bool Repository::validateUser(const std::string& username, const std::string& password) {
	std::lock_guard<std::mutex> lock(dbMutex);
	std::ifstream in(userFile);
	std::string u{ "" }, p{ "" };
	while (in >> u >> p) {
		if (u == username && p == password) return true;
	}
	return false;
}

void Repository::addUser(const std::string& username, const std::string& password) {
	std::lock_guard<std::mutex> lock(dbMutex);
	std::ofstream out(userFile, std::ios::app);
	out << username << " " << password << "\n";
}

void Repository::saveMessage(const std::string& from, const std::string& to, const std::string& msg, const std::string& group) {
	std::lock_guard<std::mutex> lock(dbMutex);
	std::ofstream out(msgFile, std::ios::app);
	out << from << "|" << to << "|" << group << "|" << msg << "\n";
}