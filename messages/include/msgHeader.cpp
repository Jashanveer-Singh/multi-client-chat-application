#include "pch.h"
#include "msgHeader.h"
namespace message
{

	msgHeader::msgHeader() : msgType(0), msgSize(0) {}
	msgHeader::msgHeader(uint8_t type, uint8_t size) : msgType(type), msgSize(size) {}
	bool msgHeader::toBytes(std::string& buffer) {
		buffer.push_back(msgType);
		buffer.push_back(msgSize);
		return true;
	}
	bool msgHeader::fromBytes(const char* buffer) {
		msgType = static_cast<uint8_t>(buffer[0]);
		msgSize = static_cast<uint8_t>(buffer[1]);
		return true;
	}
	uint8_t msgHeader::getMsgType() {
		return msgType;
	}
	uint8_t msgHeader::getMsgSize() {
		return msgSize;
	}

}