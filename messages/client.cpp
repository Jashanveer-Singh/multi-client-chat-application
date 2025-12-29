#include "pch.h"

#include "client.h"

#ifdef _DEBUG
#include <cassert>
#endif

namespace message {

	namespace client {


		Auth::Auth(CLIENTMSGTYPE type) : type(type) {}
		Auth::Auth(CLIENTMSGTYPE type, std::string username, std::string password)
			: type(type), username(username), password(password) {
		}
		bool Auth::toBytes(std::string& buffer) {
			uint8_t size = getMsgSize();

			buffer.reserve(size + buffer.size());
			buffer.push_back(static_cast<uint8_t>(username.size()));
			buffer.append(username);
			buffer.push_back(static_cast<uint8_t>(password.size()));
			buffer.append(password);
#ifdef _DEBUG
			// assert(buffer.size() == size + 1);
#endif

			return true;
		}
		bool Auth::fromBytes(const char* buffer) {
			uint8_t username_size = buffer[0];
			username.resize(username_size);
			std::memcpy(username.data(), buffer + 1, username_size);
			uint8_t password_size = buffer[username_size + 1];
			password.resize(password_size);
			std::memcpy(password.data(), buffer + 2 + username_size, password_size);
			return true;
		}
		uint8_t Auth::getMsgSize() {
			return username.size() + password.size() + 2; // for assigning length of username and password
		}
		std::string Auth::getUsername() {
			return username;
		}
		std::string Auth::getPassword() {
			return password;
		}


		Login::Login() : Auth(CLIENTMSGTYPE::LOGIN) {}
		Login::Login(std::string username, std::string password)
			: Auth(CLIENTMSGTYPE::LOGIN, username, password) {
		}

		Register::Register() : Auth(CLIENTMSGTYPE::REGISTER) {}
		Register::Register(std::string username, std::string password) 
			: Auth(CLIENTMSGTYPE::REGISTER, username, password) {}

//		Sync::Sync() = default;
//		bool Sync::toBytes(std::string& buffer) {
//		}
//		bool Sync::fromBytes(const char* buffer) {
//		}
//		uint8_t Sync::getMsgSize() {
//		}

		Message::Message()= default;
		Message::Message(std::string recipient, std::string group, std::string msg)
			: recipient(recipient), group(group), msg(msg) {
		}
		bool Message::toBytes(std::string& buffer) {
			buffer.reserve(buffer.size() + getMsgSize());

			bool forGroup = recipient.size() == 0;

			buffer.push_back(
				static_cast<uint8_t>(forGroup)
			);
			buffer.push_back(
				static_cast<uint8_t>(forGroup ? group.size() : recipient.size())
			);
			buffer.append(forGroup ? group : recipient);
			buffer.push_back(static_cast<uint8_t>(msg.size()));
			buffer.append(msg);

#ifdef _DEBUG
			// assert(buffer.size() == getMsgSize() + 3);
#endif
			return true;
		}
		bool Message::fromBytes(const char* buffer) {
			bool forGroup = static_cast<bool>(buffer[0]);
			uint8_t size = buffer[1];
			forGroup ?
				group.resize(size) :
				recipient.resize(size);
			std::memcpy(forGroup?group.data() : recipient.data(),
				buffer + 2, size);
			uint8_t msgSize = static_cast<uint8_t>(buffer [2+size]);
			msg.resize(msgSize);
			std::memcpy(msg.data(), buffer + 3+size, msgSize);
			return true;
		}
		uint8_t Message::getMsgSize() {
			return msg.size() + recipient.size()+group.size() + 3;
		}
		std::string Message::getRecipient() {
			return recipient;
		}
		std::string Message::getGroup() {
			return group;
		}
		std::string Message::getMsg() {
			return msg;
		}

		KeepAlive::KeepAlive() = default;
		bool KeepAlive::toBytes(std::string& buffer) {
			return true;
		}
		bool KeepAlive::fromBytes(const char* buffer) {
			return  true;
		}
		uint8_t KeepAlive::getMsgSize() {
			return 0;
		}
	}
}