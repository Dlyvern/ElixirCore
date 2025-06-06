#include "Logger.hpp"

#include <cstring>
#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

elix::Logger& elix::Logger::instance()
{
    static elix::Logger logger;
    return logger;
}

const std::vector<elix::Logger::Message>& elix::Logger::getMessages() const
{
    return m_messages;
}

void elix::Logger::logInternal(LogLevel level, const std::string &message, const char *file, int line, const char *function)
{
    std::string formatted = formatMessage(level, message, file, line, function);

    //TODO write shit to terminal
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (level != LogLevel::INFO)
        {
            formatted.insert(0, "\033[33m");
            formatted += "\033[0m";
        }

        (level == LogLevel::INFO ? std::cout : std::cerr) << formatted << std::endl;
    }

    // Thread-safe message storage
    glm::vec3 color;

    switch(level)
    {
        case LogLevel::INFO: color = {0.8f, 0.8f, 0.8f}; break;
        case LogLevel::WARNING: color = {1.0f, 0.8f, 0.0f}; break;
        case LogLevel::ERROR: color = {1.0f, 0.3f, 0.3f}; break;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.emplace_back(formatted, color, std::chrono::system_clock::now());
        if (m_messages.size() > 1000) {
            m_messages.erase(m_messages.begin());
        }
    }
}

std::string elix::Logger::formatMessage(LogLevel level, const std::string &message, const char* file, int line, const char* function)
{
    std::stringstream ss;

    const auto now = std::chrono::system_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    const auto timer = std::chrono::system_clock::to_time_t(now);

    ss << "[" << std::put_time(std::localtime(&timer), "%H:%M:%S.")
    << std::setfill('0') << std::setw(3) << ms.count() << "] ";

    switch (level)
    {
        case LogLevel::INFO: ss << "[INFO]"; break;
        case LogLevel::WARNING: ss << "[WARN]"; break;
        case LogLevel::ERROR: ss << "[ERROR]"; break;
    }

    if (file && line > 0 && function)
    {
        const char* filename = strrchr(file, '/') ? strrchr(file, '/') + 1 :
                      (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : file);
        ss << "[" << filename << ":" << line << "][" << function << "] ";
    }

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
