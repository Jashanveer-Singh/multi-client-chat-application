#pragma once

#include <string>

#include "enums.h"
namespace message {

	namespace server {

		
		class Message {
			std::string sender;
			std::string group;
			std::string msg;
		public:
			Message();
			Message(std::string sender, std::string group, std::string msg);
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
			std::string getSender();
			std::string getGroup();
			std::string getMsg();
		};
//		class HeartBeat
//		{
//			HeartBeat();
//			bool toBytes(std::string& buffer);
//			bool fromBytes(const char* buffer);
//			uint8_t getMsgSize();
//		};
		class OkResponse {
		public:
			OkResponse();
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
		};
		class FailureResponse
		{
		public:
			FailureResponse();
			bool toBytes(std::string& buffer);
			bool fromBytes(const char* buffer);
			uint8_t getMsgSize();
		};
//		class Push
//		{
//		};
	}
}