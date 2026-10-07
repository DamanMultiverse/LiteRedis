#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>

class LiteRedis {
private:
    int capacity;
    std::list<std::pair<std::string, std::string>> items;
    std::unordered_map<std::string, decltype(items.begin())> cache;
    mutable std::shared_mutex rw_lock;

public:
    explicit LiteRedis(int cap);
    std::string peek(const std::string& key);
    void put(const std::string& key, const std::string& value);
    int size() const;
};

#endif // LITEREDIS_H