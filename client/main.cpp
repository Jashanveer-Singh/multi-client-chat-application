
#include "client.h"
#include <iostream>

int main() {
	Client client;
	if (!client.start()) {
		return -1;
	}
    client.setRecipient();
    std::string msg;
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, msg);
//        if (msg == "exit") {
//            client.sendMessage(msg);
//            break;
//        }
		if (msg.empty()) continue;
        client.sendMessage(msg);
    }

	client.stop();
    return 0;
}