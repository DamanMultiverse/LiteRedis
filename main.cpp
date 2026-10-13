#include "LiteRedis.h"
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

#define PORT 6380

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
    
    std::cout << "[SERVER] Client disconnected.\n";
    close(client_socket);
}

int main() {
    LiteRedis db(100);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed"); exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed"); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed"); exit(EXIT_FAILURE);
    }

    std::cout << "🚀 LiteRedis Server Started!\n";
    std::cout << "📡 Listening on PORT " << PORT << "...\n";
    std::cout << "------------------------------------------------\n";

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed"); continue;
        }
        std::cout << "[SERVER] New client connected!\n";
        
        // Dispatch new thread for concurrent client handling
        std::thread(handleClient, new_socket, std::ref(db)).detach();
    }
    
    return 0;
}