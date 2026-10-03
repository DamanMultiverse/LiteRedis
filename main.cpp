#include <iostream>
#include "LiteRedis.h" 

using namespace std;

int main() {
    LiteRedis db(2); 

    cout << "--- Testing SET and LRU ---" << endl;
    db.set("user1", "Daman");
    db.set("user2", "Riya");
    db.get("user1"); 
    db.set("user3", "Aman"); 
    db.get("user2"); 

    cout << "\n--- Testing DELETE ---" << endl;
    db.del("user1"); 
    db.get("user1"); 
    db.del("unknown"); 

    return 0;
}