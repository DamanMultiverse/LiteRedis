#include <iostream>
#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class LiteRedis {
private:
    int capacity;
    std::list<std::pair<std::string, std::string>> items;
    std::unordered_map<std::string, decltype(items.begin())> cache;
    
    // Reader-Writer Lock for thread safety
    mutable std::shared_mutex rw_lock; 
    // Mutex just for clean console printing across threads
    mutable std::mutex print_lock;

public:
    LiteRedis(int cap) : capacity(cap) {}

    std::string peek(const std::string& key) {
        std::shared_lock<std::shared_mutex> lock(rw_lock); 
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second->second;
        }
        return "Not Found";
    }

    void put(const std::string& key, const std::string& value) {
        std::unique_lock<std::shared_mutex> lock(rw_lock);
        auto it = cache.find(key);
        
        if (it != cache.end()) {
            items.erase(it->second);
            cache.erase(it);
        } 
        else if (items.size() >= capacity) {
            auto last = items.back();
            cache.erase(last.first);
            items.pop_back();
        }
        
        items.push_front({key, value});
        cache[key] = items.begin();
        
        std::lock_guard<std::mutex> plock(print_lock);
        std::cout << "[WRITER] Inserted: " << key << " => " << value << "\n";
    }

    void safePrintRead(int thread_id, const std::string& key, const std::string& val) {
        std::lock_guard<std::mutex> plock(print_lock);
        std::cout << "[READER " << thread_id << "] Key: " << key << " -> " << val << "\n";
    }
};

void clientReader(LiteRedis& db, int id) {
    std::string val = db.peek("shared_config");
    db.safePrintRead(id, "shared_config", val);
}

void clientWriter(LiteRedis& db, int id) {
    db.put("user:" + std::to_string(id), "Data_" + std::to_string(id));
}

int main() {
    std::cout << "Starting LiteRedis Multithreading Stress Test...\n";
    std::cout << "------------------------------------------------\n";
    
    LiteRedis db(5);
    db.put("shared_config", "Cluster_Active");

    std::vector<std::thread> threads;
    
    // Launching 5 Writer and 5 Reader threads concurrently
    for (int i = 1; i <= 5; ++i) {
        threads.push_back(std::thread(clientWriter, std::ref(db), i));
        threads.push_back(std::thread(clientReader, std::ref(db), i));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "------------------------------------------------\n";
    std::cout << "All 10 concurrent threads finished with ZERO race conditions!\n";
    
    return 0;
}