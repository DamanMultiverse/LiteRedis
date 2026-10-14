#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    const std::string VERSION = "1.0.0"; // New version variable
    constexpr int SERVER_PORT = 6380;
    constexpr int MAX_CACHE_CAPACITY = 100;
    const std::string RDB_FILENAME = "dump.rdb";
    const std::string LOG_FILENAME = "server.log";
}

#endif 