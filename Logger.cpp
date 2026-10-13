#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

// Define the static mutex
std::mutex Logger::log_mutex;

void Logger::log(LogLevel level, const std::string& message) {
    // Thread-safe lock: only one thread can print at a time
    std::lock_guard<std::mutex> lock(log_mutex);
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    // Print timestamp
    std::cout << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] ";

    // Print Log Level
    switch (level) {
        case LogLevel::INFO:
            std::cout << "[INFO] ";
            break;
        case LogLevel::WARNING:
            std::cout << "[WARNING] ";
            break;
        case LogLevel::ERROR:
            std::cout << "[ERROR] ";
            break;
    }

    // Print the actual message
    std::cout << message << "\n";
}