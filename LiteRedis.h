#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <atomic>
#include <chrono> // Naya header time tracking ke liye

// Custom struct taaki hum value ke sath expiry time bhi save kar sakein
struct CacheNode {
    std::string key;
    std::string value;
    std::chrono::time_point<std::chrono::steady_clock> expiry; 
};

class LiteRedis {
private:
    int capacity;
    std::list<CacheNode> items; // Pair ki jagah ab humara naya struct use hoga
    std::unordered_map<std::string, decltype(items.begin())> cache;
    mutable std::shared_mutex rw_lock;

    mutable std::atomic<int> hits{0};
    mutable std::atomic<int> misses{0};

public:
    explicit LiteRedis(int cap);
    std::string peek(const std::string& key);
    std::string get(const std::string& key);
    
    // put() ab ek optional 3rd parameter lega (ttl_seconds). 0 matlab kabhi expire nahi hoga.
    void put(const std::string& key, const std::string& value, int ttl_seconds = 0);
    
    bool del(const std::string& key);
    bool exists(const std::string& key);
    void clear();
    int size() const;
    void printStats() const;
};

#endif // LITEREDIS_H