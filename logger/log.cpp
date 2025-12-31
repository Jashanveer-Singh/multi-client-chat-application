#include "log.h"

Logger::Logger(std::ostream& outputStream, std::string&& prefix)
	: out(outputStream), prefix(std::move(prefix)) {
}

void Logger::log(const std::string& message) {
	out << prefix << message << std::endl;
}