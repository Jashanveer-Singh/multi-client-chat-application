#include <iostream>
#include <conio.h>
#include "ui.h"

std::string UI::input = "";
std::mutex UI::uiMutex{};

void UI::clearScreen() {
	system("cls");
}

void UI::waitForEnter() {
	std::cout << "Press Enter to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

char UI::getChar() {
	char ch{ '\0' };
	std::cin.get(ch);
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	return ch;
}

std::string UI::getLine() {

	char ch{ '\0' };
	while (true) {
		if (_kbhit())
		{
			ch = _getch();
			if (ch == '\r') { // Enter key
				std::cout << std::endl;
				break;
			}
			else if (ch == '\b') { // Backspace key
				if (!input.empty()) {
					input.pop_back();
					std::cout << "\b \b" << std::flush;
				}
			}
			else {
				input += ch;
				std::cout << ch << std::flush;
			}
		}
	}
	std::string line = input;
	input.clear();
	return line;
}

std::string UI::getString() {
	char ch{ '\0' };
	while (true) {
		if (_kbhit())
			ch = _getch();
		{
			if (ch == '\r') { // Enter key
				std::cout << std::endl;
				break;
			}
			else if (ch == '\b') { // Backspace key
				if (!input.empty()) {
					input.pop_back();
					std::cout << "\b \b" << std::flush;
				}
			}
			else if (ch == ' ') {
				std::cout << ch << std::flush;
				break;
			}
			else {
				input += ch;
				std::cout << ch << std::flush;
			}
		}
	}
	std::string word = input;
	input.clear();
	return word;
}

void UI::print(const std::string& msg) {
	std::cout << msg;
}

void UI::println(const std::string& msg) {
	std::cout << msg << std::endl;
}

void UI::displayMessage(const std::string& msg) {
	// clear line
	std::cout << "\33[2K\r" << msg << '\n';
	prompt();
}
void UI::prompt() {
	std::cout << "You: " << input << std::flush;
}