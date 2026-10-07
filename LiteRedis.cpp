#include "LiteRedis.h"
#include <iomanip>

LiteRedis::LiteRedis(int cap) : capacity(cap) {}

std::string LiteRedis::peek(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it != cache.end()) {
        hits++;
        return it->second->second;
    }
    misses++;
    return "Not Found";
}

std::string LiteRedis::get(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) {
        misses++;
        return "Not Found";
    }
    hits++;
    items.splice(items.begin(), items, it->second);
    return it->second->second;
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

bool LiteRedis::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) {
        return false;
    }
    items.erase(it->second);
    cache.erase(it);
    std::cout << "[DEL] Removed key: " << key << "\n";
    return true;
}

bool LiteRedis::exists(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    return cache.find(key) != cache.end();
}

void LiteRedis::clear() {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    items.clear();
    cache.clear();
    hits = 0;
    misses = 0;
    std::cout << "[FLUSH] Cache cleared completely.\n";
}

int LiteRedis::size() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    return items.size();
}

void LiteRedis::printStats() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    int h = hits.load();
    int m = misses.load();
    int total = h + m;
    double hit_rate = (total > 0) ? (static_cast<double>(h) / total) * 100.0 : 0.0;

    std::cout << "\n========= LiteRedis Telemetry Stats =========\n";
    std::cout << "Current Size : " << items.size() << " / " << capacity << "\n";
    std::cout << "Cache Hits   : " << h << "\n";
    std::cout << "Cache Misses : " << m << "\n";
    std::cout << "Hit Ratio    : " << std::fixed << std::setprecision(2) << hit_rate << "%\n";
    std::cout << "=============================================\n";
}