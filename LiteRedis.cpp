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
    
    // Yahan aayega hamara Reader-Writer Lock!
    mutable std::shared_mutex rw_lock; 

public:
    LiteRedis(int cap) : capacity(cap) {}

    // PEEK: Sirf read karne ke liye. 'shared_lock' allow karta hai 
    // ki multiple threads ek sath data read kar sakein bina block hue.
    std::string peek(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(rw_lock); 
        
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second->second;
        }
        return "Not Found";
    }

    // PUT: Naya data likhne ke liye. 'unique_lock' baaki sab threads ko block 
    // kar dega jab tak yeh akela thread data insert na kar le. (Thread-Safety!)
    void put(const std::string& key, const std::string& value) {
        std::unique_lock<std::shared_mutex> lock(rw_lock);
        
        // Basic insertion logic (Aage hum yahan LRU eviction rule lagayenge)
        items.push_front({key, value});
        cache[key] = items.begin();
        std::cout << "[LOG] Inserted: " << key << " => " << value << "\n";
    }
};

int main() {
    std::cout << "Starting LiteRedis Engine on Ubuntu WSL...\n";
    std::cout << "-------------------------------------------\n";
    
    LiteRedis db(3);
    
    db.put("student:1", "Damanjeet (DR-1)");
    db.put("status", "SDE Prep Mode On!");
    
    std::cout << "Querying student:1 -> " << db.peek("student:1") << "\n";
    
    return 0;
}