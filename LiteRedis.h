#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
// #include <mutex> // NOTE: Uncomment this in Linux/Production environment

// Class declaration for our in-memory database
class LiteRedis {
private:
    int capacity;
    std::list<std::string> lruList;
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> store;
    
    // NOTE: Commented out for local Windows compilation. Uncomment in Linux.
    // mutable std::mutex mtx; 

public:
    // Constructor
    LiteRedis(int cap);

    // Core operations
    void set(std::string key, std::string value);
    void get(std::string key);
    void del(std::string key);
};

#endif