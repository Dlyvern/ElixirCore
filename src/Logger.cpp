#include "Logger.hpp"
#include <sstream>
#include <iostream>
#include <ctime>

elix::Logger& elix::Logger::instance()
{
    static elix::Logger logger;
    return logger;
}

void elix::Logger::log(LogLevel logLevel, const std::string &message, const char* file, int line, const char* function)
{
    const std::string formatted = formatMessage(logLevel, message, file, line, function);

    if (logLevel == LogLevel::INFO)
        std::cout << formatted << std::endl;
    else
        std::cerr << formatted << std::endl;
}

std::string elix::Logger::formatMessage(LogLevel level, const std::string &message, const char* file, int line, const char* function)
{
    std::stringstream ss;
    ss << "[" << getTimestamp() << "] ";

    switch (level)
    {
        case LogLevel::INFO: ss << "[INFO] "; break;
        case LogLevel::WARNING: ss << "[WARN] "; break;
        case LogLevel::ERROR: ss << "[ERROR] "; break;
    }

    if (file && line > 0 && function)
        ss << "[" << file << ":" << line << " in " << function << "] ";

    ss << message;

    return ss.str();
}

std::string elix::Logger::getTimestamp()
{
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", tm);
    return std::string(buffer);
}
