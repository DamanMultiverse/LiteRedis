#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;
class LiteRedis{
private :
    unordered_map<string,string> store ; // For storing the data and fetching it in O(1) time complexity
public:
    // Method to store or update a key-value pair (SET command)
    void set(string key, string value) {
        store[key] = value;
        cout << "OK" << endl;
    }

    // Method to retrieve a value using its key (GET command)
    void get(string key) {
        // Check if the key exists in our database
        if (store.find(key) != store.end()) {
            cout << store[key] << endl;
        } else {
            cout << "(nil)" << endl; // Standard Redis output when key is not found
        }
    }

    // Method to remove a key-value pair (DELETE command)
    void del(string key) {
        // erase() returns 1 if deleted, 0 if the key was not found
        if (store.erase(key)) {
            cout << "(integer) 1" << endl; 
        } else {
            cout << "(integer) 0" << endl; 
        }
    }
};

int main() {
    LiteRedis db;

    db.set("name", "Damanjeet");
    db.get("name");
    db.del("name");
    db.get("name");

    return 0;
}