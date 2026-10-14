#include "LiteRedis.h"
#include "TcpServer.h"
#include "Logger.h"
#include "Config.h"
#include <csignal>
#include <cstdlib>

// Global pointers for the signal handler to access resources during graceful shutdown
LiteRedis* global_db_ptr = nullptr;
TcpServer* global_server_ptr = nullptr;

void handleSignal(int signum) {
    Logger::log(LogLevel::WARNING, "Interrupt signal (" + std::to_string(signum) + ") received. Initiating graceful shutdown...");
    
    // Stop the TCP Server gracefully
    if (global_server_ptr != nullptr) {
        global_server_ptr->stop();
    }
    
    // Persist data to disk before exiting
    if (global_db_ptr != nullptr) {
        global_db_ptr->saveToDisk();
    }
    
    Logger::log(LogLevel::INFO, "Graceful shutdown complete. Exiting safely.");
    exit(signum);
}

int main() {
    // Initialize application logging
    Logger::init(Config::LOG_FILENAME);

    // Initialize core database engine
    LiteRedis db(Config::MAX_CACHE_CAPACITY);
    global_db_ptr = &db; 

    // Initialize and configure the TCP network server
    TcpServer server(db);
    global_server_ptr = &server;

    // Register OS signal handlers for graceful termination
    signal(SIGINT, handleSignal);  
    signal(SIGTERM, handleSignal); 

    // Start the server (this call blocks and handles client connections)
    server.start();

    return 0;
}