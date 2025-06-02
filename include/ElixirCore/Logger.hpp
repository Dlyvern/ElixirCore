#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>


#define LOG_INFO(msg)     elix::Logger::instance().log(elix::Logger::LogLevel::INFO,     msg, __FILE__, __LINE__, __func__)
#define LOG_WARN(msg)     elix::Logger::instance().log(elix::Logger::LogLevel::WARNING,     msg, __FILE__, __LINE__, __func__)
#define LOG_ERROR(msg)    elix::Logger::instance().log(elix::Logger::LogLevel::ERROR,    msg, __FILE__, __LINE__, __func__)

namespace elix
{
    class Logger
    {
    public:
        enum class LogLevel
        {
            INFO,
            WARNING,
            ERROR
        };

        static Logger& instance();

        void log(LogLevel logLevel, const std::string& message, const char* file = nullptr, int line = -1, const char* function = nullptr);

    private:
        std::string formatMessage(LogLevel level, const std::string& message, const char* file = nullptr, int line = -1, const char* function = nullptr);
        std::string getTimestamp();
        Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
    };
} //namespace elix
#endif //LOGGER_HPP
