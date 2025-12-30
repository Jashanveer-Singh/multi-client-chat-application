
#include "client.h"
#include <iostream>
#include "menu.h"
#include "ui.h"

void receiveMessage(Client& client, UI& ui) {
    while (client.isRunning()) {
        std::string msg = client.receiveMessage();
        if (!msg.empty())
            ui.displayMessage(msg);
    }
}

int main() {
    UI ui;
	Client client;
	if (!client.start()) {  
		return -1;
	}
    mainMenu(client, ui);
    menuChooseRecipientOrGroup(client, ui);
    std::thread receiveThread(receiveMessage, std::ref(client), std::ref(ui));
    std::string msg;
    while (true) {
        ui.prompt();
        msg = ui.getLine();
		if (msg.empty()) continue;
        client.sendMessage(msg);
    }

	client.stop();
    receiveThread.join();
    return 0;
}