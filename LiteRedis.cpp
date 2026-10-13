#include "LiteRedis.h"
#include <iomanip>
#include <iostream>

LiteRedis::LiteRedis(int cap, std::string filename) : capacity(cap), dump_file(filename) {
    loadFromDisk();
}

void LiteRedis::saveToDisk() {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    std::ofstream out(dump_file, std::ios::trunc); 
    if (!out.is_open()) {
        std::cerr << "[ERROR] Could not open RDB file for writing!\n";
        return;
    }
    
    int saved_count = 0;
    auto now = std::chrono::steady_clock::now();
    
    for (const auto& item : items) {
        long long ttl_remaining = 0;
        
        if (item.expiry != std::chrono::time_point<std::chrono::steady_clock>::min()) {
            if (now > item.expiry) continue; 
            ttl_remaining = std::chrono::duration_cast<std::chrono::seconds>(item.expiry - now).count();
        }
        
        out << item.key << " " << item.value << " " << ttl_remaining << "\n";
        saved_count++;
    }
    
    out.close();
    std::cout << "[PERSISTENCE] " << saved_count << " keys saved to disk (" << dump_file << ").\n";
}

void LiteRedis::loadFromDisk() {
    std::ifstream in(dump_file);
    if (!in.is_open()) {
        std::cout << "[PERSISTENCE] No existing RDB file found. Starting fresh.\n";
        return;
    }
    
    std::string k, v;
    long long ttl;
    int loaded_count = 0;
    
    while (in >> k >> v >> ttl) {
        put(k, v, ttl);
        loaded_count++;
    }
    
    in.close();
    std::cout << "[PERSISTENCE] " << loaded_count << " keys loaded from disk (" << dump_file << ").\n";
}

std::string LiteRedis::peek(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it != cache.end()) {
        auto now = std::chrono::steady_clock::now();
        if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) {
            misses++; return "Not Found (Expired)";
        }
        hits++; return it->second->value;
    }
    misses++; return "Not Found";
}

std::string LiteRedis::get(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) { misses++; return "Not Found"; }

    auto now = std::chrono::steady_clock::now();
    if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) {
        items.erase(it->second);
        cache.erase(it);
        misses++; return "Not Found (Expired)";
    }

    hits++;
    items.splice(items.begin(), items, it->second);
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
        cache.erase(last.key);
        items.pop_back();
    }
    
    auto expiry_time = std::chrono::time_point<std::chrono::steady_clock>::min(); 
    if (ttl_seconds > 0) {
        expiry_time = std::chrono::steady_clock::now() + std::chrono::seconds(ttl_seconds);
    }
    
    items.push_front({key, value, expiry_time});
    cache[key] = items.begin();
    std::cout << "[WRITER] Inserted: " << key << " => " << value << "\n";
}

bool LiteRedis::del(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) return false;
    items.erase(it->second);
    cache.erase(it);
    std::cout << "[DEL] Removed key: " << key << "\n";
    return true;
}

bool LiteRedis::exists(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    auto it = cache.find(key);
    if (it == cache.end()) return false;
    
    auto now = std::chrono::steady_clock::now();
    if (it->second->expiry != std::chrono::time_point<std::chrono::steady_clock>::min() && now > it->second->expiry) return false;
    return true;
}

void LiteRedis::clear() {
    std::unique_lock<std::shared_mutex> lock(rw_lock);
    items.clear(); cache.clear();
    hits = 0; misses = 0;
}

int LiteRedis::size() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock); return items.size();
}

void LiteRedis::printStats() const {
    std::shared_lock<std::shared_mutex> lock(rw_lock);
    int h = hits.load(), m = misses.load(), total = h + m;
    double hit_rate = (total > 0) ? (static_cast<double>(h) / total) * 100.0 : 0.0;

    std::cout << "\n========= LiteRedis Stats =========\n";
    std::cout << "Size : " << items.size() << " / " << capacity << " | Hit Ratio: " << std::fixed << std::setprecision(2) << hit_rate << "%\n";
    std::cout << "=====================================\n";
}