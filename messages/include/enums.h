#pragma once

#include <stdint.h>

namespace message {
	enum class CLIENTMSGTYPE : uint8_t {
		INVALID = 0,
		LOGIN,
		REGISTER,
		MESSAGE,
		KEEPALIVE,
		SYNC,
	};

	enum class SERVERMSGTYPE : uint8_t {
		INVALID = 0,
		MESSAGE ,
		HEARTBEAT ,
//		OK ,
//		FAILURE ,
//		PUSH

	};
}