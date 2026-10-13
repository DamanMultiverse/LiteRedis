#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <atomic>
#include <chrono>
#include <fstream> // Naya header File I/O (Disk Persistence) ke liye

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
    
    // NAYA: Hard drive par kis file mein data save hoga uska naam
    std::string dump_file; 

public:
    // UPDATE: Constructor ab ek optional filename lega (default: dump.rdb)
    explicit LiteRedis(int cap, std::string filename = "dump.rdb");
    
    std::string peek(const std::string& key);
    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value, int ttl_seconds = 0);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    void clear();
    int size() const;
    void printStats() const;

    // --- NAYE FUNCTIONS: Data Persistence ---
    void saveToDisk(); // RAM ka data Hard Drive par save karne ke liye
    void loadFromDisk(); // Hard Drive ka data wapas RAM mein laane ke liye
};

#endif // LITEREDIS_H