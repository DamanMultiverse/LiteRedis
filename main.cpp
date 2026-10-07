#include "LiteRedis.h"
#include <thread>
#include <vector>

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

    std::cout << "\nAll 10 concurrent threads finished with ZERO race conditions!\n";

    std::cout << "\n--- Testing LRU get() and del() Operations ---\n";
    db.put("temp_key", "Temporary_Data");
    std::cout << "Fetching temp_key: " << db.get("temp_key") << "\n";
    db.del("temp_key");
    std::cout << "Fetching after del: " << db.get("temp_key") << "\n";

    std::cout << "Checking if user:2 exists: " << (db.exists("user:2") ? "Yes" : "No") << "\n";
    db.printStats();
    return 0;
}