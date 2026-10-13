#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>
#include <fstream> 

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    static std::mutex log_mutex;
    static std::ofstream log_file; 

public:
    static void init(const std::string& filename = "server.log");
    static void log(LogLevel level, const std::string& message);
    static void close();
};

#endif // LOGGER_H