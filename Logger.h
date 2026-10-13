#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

// Log levels define karne ke liye enum
enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    // Static mutex taaki poore project mein ek hi lock share ho
    static std::mutex log_mutex;

public:
    // Static function taaki bina object banaye Logger::log() call kar sakein
    static void log(LogLevel level, const std::string& message);
};

#endif // LOGGER_H