#include "LiteRedis.h"

LiteRedis::LiteRedis(int cap) : capacity(cap) {}

std::string LiteRedis::peek(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second->second;
    }
    return "Not Found";
}

void LiteRedis::put(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    
    if (it != cache.end()) {
        items.erase(it->second);
        cache.erase(it);
    } else if (items.size() >= static_cast<size_t>(capacity)) {
        auto last = items.back();
        cache.erase(last.first);
        items.pop_back();
    }
    
    items.push_front({key, value});
    cache[key] = items.begin();
    std::cout << "[WRITER] Inserted: " << key << " => " << value << "\n";
}

int LiteRedis::size() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    return items.size();
}