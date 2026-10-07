#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <atomic>

class LiteRedis {
private:
    int capacity;
    std::list<std::pair<std::string, std::string>> items;
    std::unordered_map<std::string, decltype(items.begin())> cache;
    mutable std::shared_mutex rw_lock;

    // Thread-safe atomic counters for cache telemetry
    mutable std::atomic<int> hits{0};
    mutable std::atomic<int> misses{0};

public:
    explicit LiteRedis(int cap);
    std::string peek(const std::string& key);
    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    bool del(const std::string& key);
    bool exists(const std::string& key) const;
    void clear();
    int size() const;
    void printStats() const;
};

#endif // LITEREDIS_H