#pragma once

#include <stdint.h>
#include <string>
namespace message
{

class msgHeader {
	uint8_t msgType;
	uint8_t msgSize;
public:
	msgHeader();
	msgHeader(uint8_t type, uint8_t size);
	bool toBytes(std::string& buffer);
	bool fromBytes(const char* buffer);
	uint8_t getMsgType();
	uint8_t getMsgSize();
};
}