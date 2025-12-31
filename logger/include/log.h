#pragma once

#include <ostream>

class Logger {
	std::ostream& out;
	std::string prefix;
	public:
		Logger(std::ostream& outputStream, std::string&& prefix);

		void log(const std::string& message);
};