#ifndef LITEREDIS_H
#define LITEREDIS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>

// Class declaration for our in-memory database
class LiteRedis {
private:
    int capacity;
    std::list<std::string> lruList;
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> store;

public:
    // Constructor
    LiteRedis(int cap);

    // Core operations
    void set(std::string key, std::string value);
    void get(std::string key);
    void del(std::string key);
};

#endif