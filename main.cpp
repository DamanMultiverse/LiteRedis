#include <iostream>
#include <string>
#include <unordered_map>
#include <list> // Added for Doubly Linked List

using namespace std;

// Class representing our custom in-memory database with LRU Cache
class LiteRedis {
private:
    int capacity; // Maximum number of keys the database can hold
    list<string> lruList; // Doubly linked list to track recent usage (front = most recent, back = least recent)
    // Hash map stores key and a pair containing its value and its exact position (iterator) in the list
    unordered_map<string, pair<string, list<string>::iterator>> store;

public:
    // Constructor to set database capacity
    LiteRedis(int cap) {
        capacity = cap;
    }

    // Method to store or update a key-value pair (SET command)
    void set(string key, string value) {
        // If key already exists, remove it from its current position in the list
        if (store.find(key) != store.end()) {
            lruList.erase(store[key].second);
        } 
        // If database is full, remove the least recently used item (from the back of the list)
        else if (store.size() == capacity) {
            string lruKey = lruList.back();
            lruList.pop_back();
            store.erase(lruKey);
        }

        // Add the new key to the front of the list (most recently used)
        lruList.push_front(key);
        // Store the value and the iterator pointing to its position in the list
        store[key] = {value, lruList.begin()};
        
        cout << "OK" << endl;
    }

    // Method to retrieve a value using its key (GET command)
    void get(string key) {
        // Check if the key exists
        if (store.find(key) != store.end()) {
            // Update its usage: move it to the front of the list
            lruList.erase(store[key].second);
            lruList.push_front(key);
            // Update the stored iterator to the new position
            store[key].second = lruList.begin();
            
            cout << store[key].first << endl;
        } else {
            cout << "(nil)" << endl; 
        }
    }
};

int main() {
    // Initialize database with a strict capacity of 2 items
    LiteRedis db(2);

    cout << "Adding user1..." << endl;
    db.set("user1", "Daman"); // Cache currently holds: [user1]
    
    cout << "Adding user2..." << endl;
    db.set("user2", "Riya");  // Cache currently holds: [user2, user1]
    
    cout << "Fetching user1 (makes it most recent)..." << endl;
    db.get("user1");          // Cache currently holds: [user1, user2]
    
    cout << "Adding user3 (database full, evicts least recent - user2)..." << endl;
    db.set("user3", "Aman");  // Cache currently holds: [user3, user1]
    
    cout << "Fetching user2 (should be nil because it was evicted)..." << endl;
    db.get("user2");          // Expected Output: (nil)

    return 0;
}