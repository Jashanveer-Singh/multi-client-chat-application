#pragma once

#include <string>
#include <stdint.h>
#include "enums.h"
namespace message {

	namespace client {


		class Auth
		{
			CLIENTMSGTYPE type;
		protected:
			std::string username;
			std::string password;

		public:
			Auth(CLIENTMSGTYPE type);
			Auth(CLIENTMSGTYPE type, std::string username, std::string password);
			
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
			std::string getUsername();
			std::string getPassword();
		};

		class Login : public Auth
		{
		public:
			Login();
			Login(std::string username, std::string password);
		};

		class Register : public Auth
		{
		public:
			Register();
			Register(std::string username, std::string password);
		};

//class Logout
		//{
		//	std::string username;
		//public:
		//	Logout();
		//	Logout(std::string username);
		//	bool toBytes(std::string& buffer);
		//	bool fromBytes(const char* buffer);
		//	uint8_t getMsgSize();
		//};

//		class Sync
//		{
//		public:
//			Sync();
//			bool toBytes(std::string& buffer);
//			bool fromBytes(const char* buffer);
//			uint8_t getMsgSize();
//		};
		
		
		class KeepAlive
		{
			public:
			KeepAlive();
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
		};

		class Message
		{
			std::string recipient;
			std::string group;
			std::string msg;
		public:
			Message();
			Message(std::string recipient, std::string group, std::string msg);
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
			std::string getRecipient();
			std::string getGroup();
			std::string getMsg();
		};
	}
}