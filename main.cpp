#include "LiteRedis.h"
#include "Logger.h"
#include "Config.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <sstream>
#include <vector>
#include <csignal>

// Global pointers required for the signal handler to access resources during shutdown
LiteRedis* global_db_ptr = nullptr;
int global_server_fd = -1;

void handleSignal(int signum) {
    Logger::log(LogLevel::WARNING, "Interrupt signal (" + std::to_string(signum) + ") received. Initiating graceful shutdown...");
    
    // Save current in-memory state to persistent storage
    if (global_db_ptr != nullptr) {
        global_db_ptr->saveToDisk();
    }
    
    // Close the primary server socket
    if (global_server_fd != -1) {
        close(global_server_fd);
        Logger::log(LogLevel::INFO, "Server socket closed.");
    }
    
    Logger::log(LogLevel::INFO, "Graceful shutdown complete. Exiting safely.");
    exit(signum);
}

void handleClient(int client_socket, LiteRedis& db) {
    char buffer[1024] = {0};
    std::string welcome = "🚀 Connected to LiteRedis. Commands: PING, SET <k> <v> [ttl], GET <k>, DEL <k>, SAVE, EXIT\n";
    send(client_socket, welcome.c_str(), welcome.length(), 0);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_socket, buffer, 1024);
        if (bytes_read <= 0) break; 

        std::string command(buffer);
        std::istringstream iss(command);
        std::vector<std::string> tokens;
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        std::string cmd = tokens[0];
        std::string response = "";

        if (cmd == "PING") {
            response = "+PONG\n";
        } 
        else if (cmd == "SET" && tokens.size() >= 3) {
            int ttl = (tokens.size() == 4) ? std::stoi(tokens[3]) : 0;
            db.put(tokens[1], tokens[2], ttl);
            response = "+OK\n";
        } 
        else if (cmd == "GET" && tokens.size() == 2) {
            std::string res = db.get(tokens[1]);
            response = "$" + std::to_string(res.length()) + "\r\n" + res + "\r\n";
        } 
        else if (cmd == "DEL" && tokens.size() == 2) {
            bool deleted = db.del(tokens[1]);
            response = deleted ? ":1\n" : ":0\n"; 
        }
        else if (cmd == "SAVE") {
            db.saveToDisk();
            response = "+OK (Snapshot saved to disk)\n";
        }
        else if (cmd == "EXIT" || cmd == "QUIT") {
            response = "Bye!\n";
            send(client_socket, response.c_str(), response.length(), 0);
            break; 
        }
        else {
            response = "-ERR Unknown command or wrong arguments\n";
        }

        send(client_socket, response.c_str(), response.length(), 0);
    }
    
    Logger::log(LogLevel::INFO, "Client disconnected.");
    close(client_socket);
}

int main() {
    Logger::init(Config::LOG_FILENAME);

    LiteRedis db(Config::MAX_CACHE_CAPACITY);
    global_db_ptr = &db; // Assign global pointer for signal handling

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((global_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        Logger::log(LogLevel::ERROR, "Socket creation failed"); 
        exit(EXIT_FAILURE);
    }
    if (setsockopt(global_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        Logger::log(LogLevel::ERROR, "Socket configuration failed (setsockopt)"); 
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(Config::SERVER_PORT);

    if (bind(global_server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        Logger::log(LogLevel::ERROR, "Socket bind failed"); 
        exit(EXIT_FAILURE);
    }

    if (listen(global_server_fd, 10) < 0) {
        Logger::log(LogLevel::ERROR, "Socket listen failed"); 
        exit(EXIT_FAILURE);
    }

    // Register OS signal handlers
    signal(SIGINT, handleSignal);  // Catch Ctrl+C
    signal(SIGTERM, handleSignal); // Catch termination requests

    Logger::log(LogLevel::INFO, "LiteRedis Server Started!");
    Logger::log(LogLevel::INFO, "Listening on PORT " + std::to_string(Config::SERVER_PORT) + "...");

    while (true) {
        int new_socket;
        if ((new_socket = accept(global_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            Logger::log(LogLevel::ERROR, "Client accept failed"); 
            continue;
        }
        Logger::log(LogLevel::INFO, "New client connected!");
        
        std::thread(handleClient, new_socket, std::ref(db)).detach();
    }
    
    return 0;
}