#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <sstream>

#define LOG_INFO(...) elix::Logger::logWrapper(elix::Logger::LogLevel::INFO, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_WARN(...) elix::Logger::logWrapper(elix::Logger::LogLevel::WARNING, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LOG_ERROR(...) elix::Logger::logWrapper(elix::Logger::LogLevel::ERROR, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

namespace elix
{
    class Logger
    {
    public:
        struct Message
        {
            std::string message;
            glm::vec3 color;
            std::chrono::system_clock::time_point timestamp;
        };

        enum class LogLevel
        {
            INFO,
            WARNING,
            ERROR
        };

        static Logger& instance();

        static const char* logLevelToString(LogLevel level)
        {
            switch(level)
            {
                case LogLevel::INFO: return "INFO";
                case LogLevel::WARNING: return "WARNING";
                case LogLevel::ERROR: return "ERROR";
                default: return "UNKNOWN";
            }
        }

        template<typename... Args>
        static void logWrapper(LogLevel level, Args&&... args)
        {
            if constexpr (sizeof...(Args) == 0)
                instance().logInternal(level, "", nullptr, -1, nullptr);
            else
            {
                std::ostringstream oss;
                (instance().streamArg(oss, std::forward<Args>(args)), ...);
                instance().logInternal(level, oss.str(), nullptr, -1, nullptr);
            }
        }

        template<typename... Args>
        static void logWrapper(LogLevel level, const char* file, int line, const char* function, Args&&... args)
        {
            std::ostringstream oss;
            (instance().streamArg(oss, std::forward<Args>(args)), ...);
            instance().logInternal(level, oss.str(), file, line, function);
        }

        [[nodiscard]] const std::vector<Message>& getMessages() const;

    private:
        std::mutex m_mutex;

        std::vector<Message> m_messages;

        template<typename T>
        void streamArg(std::ostringstream& oss, T&& arg)
        {
            if constexpr (std::is_enum_v<std::decay_t<T>>)
                oss << static_cast<std::underlying_type_t<std::decay_t<T>>>(arg);
            else
                oss << std::forward<T>(arg);
        }

        void logInternal(LogLevel level, const std::string& message, const char* file, int line, const char* function);

        std::string formatMessage(LogLevel level, const std::string& message, const char* file = nullptr, int line = -1, const char* function = nullptr);
        std::string getTimestamp();
        Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
    };
} //namespace elix
#endif //LOGGER_HPP
