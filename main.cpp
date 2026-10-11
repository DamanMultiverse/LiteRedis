#include "LiteRedis.h"
#include <thread>
#include <vector>
#include <chrono> // Time aur sleep functionality ke liye

void simulateClientReader(LiteRedis& db, int id) {
    std::string res = db.peek("shared_config");
    std::cout << "[READER " << id << "] Key: shared_config -> " << res << "\n";
}

void simulateClientWriter(LiteRedis& db, int id) {
    db.put("user:" + std::to_string(id), "Data_" + std::to_string(id));
}

int main() {
    std::cout << "Starting Modular LiteRedis Multithreading Stress Test...\n";
    std::cout << "--------------------------------------------------------\n";
    
    LiteRedis db(5);
    db.put("shared_config", "Cluster_Active");

    std::vector<std::thread> threads;
    for (int i = 1; i <= 5; ++i) {
        threads.push_back(std::thread(simulateClientWriter, std::ref(db), i));
        threads.push_back(std::thread(simulateClientReader, std::ref(db), i));
    }

    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "\n--- Testing LRU get() and del() Operations ---\n";
    db.put("temp_key", "Temporary_Data");
    std::cout << "Fetching temp_key: " << db.get("temp_key") << "\n";
    db.del("temp_key");
    std::cout << "Fetching after del: " << db.get("temp_key") << "\n";

    // --- NAYA TTL TEST BLOCK ---
    std::cout << "\n--- Testing TTL (Time-To-Live) Feature ---\n";
    
    // 2 seconds ke TTL ke sath data daalo
    db.put("session_token", "abc123xyz", 2); 
    
    // Turant fetch karke dekho (Milna chahiye)
    std::cout << "Fetching immediately: " << db.get("session_token") << "\n";
    
    std::cout << "Waiting for 3 seconds...\n";
    // Program ko 3 seconds ke liye rok do
    std::this_thread::sleep_for(std::chrono::seconds(3)); 
    
    // 3 seconds baad fetch karke dekho (Expire ho jana chahiye)
    std::cout << "Fetching after 3 seconds: " << db.get("session_token") << "\n";

    db.printStats();
    
    return 0;
}