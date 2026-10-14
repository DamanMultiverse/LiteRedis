#ifndef LITEREDIS_H
#define LITEREDIS_H

#include "Config.h"
#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <atomic>
#include <chrono>
#include <fstream> 

struct CacheNode {
    std::string key;
    std::string value;
    std::chrono::time_point<std::chrono::steady_clock> expiry; 
};

class LiteRedis {
private:
    int capacity;
    std::list<CacheNode> items;
    std::unordered_map<std::string, decltype(items.begin())> cache;
    mutable std::shared_mutex rw_lock;

    mutable std::atomic<int> hits{0};
    mutable std::atomic<int> misses{0};
    
    std::string dump_file; 

public:
   explicit LiteRedis(int cap = Config::MAX_CACHE_CAPACITY, std::string filename = Config::RDB_FILENAME);
    
    // Modern C++17 attributes to prevent unused return values
    [[nodiscard]] std::string peek(const std::string& key);
    [[nodiscard]] std::string get(const std::string& key);
    [[nodiscard]] bool exists(const std::string& key);
    [[nodiscard]] int size() const;
    
    void put(const std::string& key, const std::string& value, int ttl_seconds = 0);
    bool del(const std::string& key);
    void clear();
    void printStats() const;

    void saveToDisk(); 
    void loadFromDisk();
};

#endif // LITEREDIS_H