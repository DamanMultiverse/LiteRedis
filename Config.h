#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Centralized configuration namespace to prevent hardcoded magic numbers
namespace Config {
    constexpr int SERVER_PORT = 6380;
    constexpr int MAX_CACHE_CAPACITY = 100;
    const std::string RDB_FILENAME = "dump.rdb";
    const std::string LOG_FILENAME = "server.log";
}

#endif // CONFIG_H