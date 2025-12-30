#pragma once
#include <string>
#include <mutex>

class UI {
	static std::string input;
	static std::mutex uiMutex;
public:
	char getChar();
	std::string getLine();
	std::string getString();
	void clearScreen();
	void waitForEnter();
	void print(const std::string& msg);
	void println(const std::string& msg);
	void displayMessage(const std::string& msg);
	void prompt();
};