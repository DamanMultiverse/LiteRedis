#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

// Initialize static members
std::mutex Logger::log_mutex;
std::ofstream Logger::log_file;

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex);
    if (!log_file.is_open()) {
        // Open file in append mode to preserve historical logs
        log_file.open(filename, std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "[ERROR] Failed to open log file: " << filename << "\n";
        }
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    std::string level_str;
    switch (level) {
        case LogLevel::INFO:    level_str = "[INFO] "; break;
        case LogLevel::WARNING: level_str = "[WARNING] "; break;
        case LogLevel::ERROR:   level_str = "[ERROR] "; break;
    }

    // Output to standard console
    std::cout << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] " 
              << level_str << message << "\n";

    // Output to persistent log file if initialized
    if (log_file.is_open()) {
        log_file << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] " 
                 << level_str << message << "\n";
        log_file.flush(); // Ensure immediate write to disk preventing data loss on crash
    }
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_file.is_open()) {
        log_file.close();
    }
}