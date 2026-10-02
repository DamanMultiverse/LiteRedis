#include "LiteRedis.h"

// Constructor implementation
LiteRedis::LiteRedis(int cap) {
    capacity = cap;
}

// SET operation implementation
void LiteRedis::set(std::string key, std::string value) {
    if (store.find(key) != store.end()) {
        lruList.erase(store[key].second);
    } else if (store.size() == capacity) {
        std::string lruKey = lruList.back();
        lruList.pop_back();
        store.erase(lruKey);
    }
    
    lruList.push_front(key);
    store[key] = {value, lruList.begin()};
    std::cout << "OK" << std::endl;
}

// GET operation implementation
void LiteRedis::get(std::string key) {
    if (store.find(key) != store.end()) {
        lruList.erase(store[key].second);
        lruList.push_front(key);
        store[key].second = lruList.begin();
        
        std::cout << store[key].first << std::endl;
    } else {
        std::cout << "(nil)" << std::endl;
    }
}

// DELETE operation implementation
void LiteRedis::del(std::string key) {
    if (store.find(key) != store.end()) {
        // Remove from the doubly linked list in O(1) time
        lruList.erase(store[key].second);
        // Remove from the hash map in O(1) time
        store.erase(key);
        std::cout << "(integer) 1" << std::endl;
    } else {
        std::cout << "(integer) 0" << std::endl;
    }
}