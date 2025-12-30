#include "menu.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <iostream>
#include <limits>

void mainMenu(Client& client, UI ui) {
    while (true) {
        char choice;
        std::string username, password;

        system("cls");
        ui.println("1. Login");
        ui.println("2. Register");
        ui.print("Choose an option: ");
		choice = ui.getChar();
		if (choice != '1' && choice != '2') {
            ui.println("Invalid choice. Press Enter to try again.");
            ui.waitForEnter();
            continue;
        }
		ui.print("Enter username: ");
		username = ui.getLine();

		ui.print("Enter password: ");
		password = ui.getLine();

        if (choice == '1') {
            if (client.login(username, password)) {
				ui.println("Login successful. Press Enter to continue.");
				ui.waitForEnter();
                break;
            }
			ui.println("Login failed. Press Enter to try again.");
			ui.waitForEnter();
        }
        else if (choice == '2') {
            if (client.Register(username, password)) {
				ui.println("Registration successful. Press Enter to continue.");
				ui.waitForEnter();
                break;
            }
			ui.println("Registration failed. Press Enter to try again.");
			ui.waitForEnter();
        }
    }
}

void menuSetRecipient(Client& client, UI ui) {
	ui.print("Enter recipient username: ");
	std::string recipient = ui.getLine();
	client.setRecipient(recipient);
}

void menuCreateGroup(Client& client, UI ui) {
    ui.print("Enter group name: ");
    std::string groupName = ui.getLine();
    ui.print("Enter members (comma separated usernames): ");
    std::string membersLine = ui.getLine();
    std::vector<std::string> members;
    size_t pos = 0;
    while ((pos = membersLine.find(',')) != std::string::npos) {
        members.push_back(membersLine.substr(0, pos));
        membersLine.erase(0, pos + 1);
    }
    members.push_back(membersLine); // Add last member
    if (client.createGroup(groupName, members)) {
        ui.println("Group created successfully.");
    } else {
        ui.println("Failed to create group.");
    }
}

void menuChooseRecipientOrGroup(Client& client, UI ui) {
    while (true) {
        char choice;
        ui.println("1. Set Recipient");
        ui.println("2. Create Group");
        ui.println("3. Choose Group");
        ui.print("Choose an option: ");
        choice = ui.getChar();
        if (choice == '1') {
            menuSetRecipient(client, ui);
            break;
        } else if (choice == '2') {
            menuCreateGroup(client, ui);
            break;
        }
        else if (choice == '3'){
			ui.print("Enter group name: ");
			std::string groupName = ui.getLine();
			client.setGroup(groupName);
			break;
        }
        else {
            ui.println("Invalid choice. Press Enter to try again.");
            ui.waitForEnter();
        }
    }
}   