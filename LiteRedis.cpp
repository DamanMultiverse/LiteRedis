#include "LiteRedis.h"
#include <iomanip>

LiteRedis::LiteRedis(int cap) : capacity(cap) {}

std::string LiteRedis::peek(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it != cache.end()) {
        // TTL Check: Agar data ka expiry time set hai aur current time usse aage nikal gaya hai
        auto now = std::chrono::steady_clock::now();
        if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) {
            misses++;
            return "Not Found (Expired)";
        }
        hits++;
        return it->second->value; // 'second' ki jagah ab 'value' use hoga
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

    // TTL Check with Lazy Eviction
    auto now = std::chrono::steady_clock::now();
    if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) {
        // Data expire ho chuka hai, toh usko cache se memory se delete kar do
        items.erase(it->second);
        cache.erase(it);
        misses++;
        return "Not Found (Expired)";
    }

    hits++;
    items.splice(items.begin(), items, it->second); // LRU Promotion
    return it->second->value;
}

void LiteRedis::put(const std::string& key, const std::string& value, int ttl_seconds) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    
    if (it != cache.end()) {
        items.erase(it->second);
        cache.erase(it);
    } else if (items.size() >= static_cast<size_t>(capacity)) {
        auto last = items.back();
        cache.erase(last.key); // 'first' ki jagah ab 'key' use hoga
        items.pop_back();
    }
    
    // Calculate Expiry Time (Agar ttl_seconds 0 hai, toh default min() set hoga yaani kabhi expire nahi hoga)
    auto expiry_time = std::chrono::time_point<std::chrono::steady_clock>::min(); 
    if (ttl_seconds > 0) {
        expiry_time = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds);
    }
    
    // Naya CacheNode struct push kar rahe hain: {key, value, expiry_time}
    items.push_front({key, value, expiry_time});
    cache[key] = items.begin();
    std::cout << "[WRITER] Inserted: " << key << " => " << value << (ttl_seconds > 0 ? " (TTL set)" : "") << "\n";
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

bool LiteRedis::exists(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) return false;
    
    // Exists mein bhi TTL check karna zaroori hai
    auto now = std::chrono::steady_clock::now();
    if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) {
        return false;
    }
    return true;
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