#include <iostream>
#include "LiteRedis.h" // Including our custom header file

using namespace std;

int main() {
    LiteRedis db(2); // Set capacity to 2

    cout << "--- Testing SET and LRU ---" << endl;
    db.set("user1", "Daman");
    db.set("user2", "Riya");
    db.get("user1"); // Moves user1 to front
    db.set("user3", "Aman"); // Evicts user2
    db.get("user2"); // Should be (nil)

    cout << "\n--- Testing DELETE ---" << endl;
    db.del("user1"); // Should delete and return 1
    db.get("user1"); // Should be (nil) now
    db.del("unknown"); // Should return 0 (not found)

    return 0;
}