#include "pch.h"


#ifdef _DEBUG
#include <cassert>
#endif

#include "server.h"
namespace message {
	namespace server {

		Message::Message() = default;
		Message::Message(std::string sender, std::string group, std::string msg)
			: sender(sender), group(group), msg(msg) {
		}

		bool Message::toBytes(std::string& buffer) {
			uint8_t size = getMsgSize();
			buffer.reserve(size + buffer.size());
			buffer.push_back(static_cast<uint8_t>(sender.size()));
			buffer.append(sender);
			buffer.push_back(static_cast<uint8_t>(group.size()));
			buffer.append(group);
			buffer.push_back(static_cast<uint8_t>(msg.size()));
			buffer.append(msg);
#ifdef _DEBUG
			//assert(buffer.size() == size + 1);
#endif
			return true;
		}
		bool Message::fromBytes(const char* buffer) {
			uint8_t sender_size = buffer[0];
			sender.resize(sender_size);
			std::memcpy(sender.data(), buffer + 1, sender_size);
			uint8_t group_size = buffer[sender_size + 1];
			group.resize(group_size);
			std::memcpy(group.data(), buffer + 2 + sender_size, group_size);
			uint8_t msg_size = buffer[sender_size + group_size + 2];
			msg.resize(msg_size);
			std::memcpy(msg.data(), buffer + 3 + sender_size + group_size, msg_size);
			return true;
		}
		uint8_t Message::getMsgSize() {
			return sender.size() + group.size() + msg.size() + 3; // for assigning length of sender, group and msg

		}
		std::string Message::getSender() {
			return sender;
		}
		std::string Message::getGroup() {
			return group;
		}
		std::string Message::getMsg() {
			return msg;
		}
	}
}
