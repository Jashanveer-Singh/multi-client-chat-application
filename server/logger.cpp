#include <iostream>
#include "log.h"

Logger errorLog(std::cerr, "[ERROR] ");
Logger infoLog(std::cout, "[INFO] ");
Logger debugLog(std::cout, "[DEBUG] ");