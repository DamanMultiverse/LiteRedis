#include "LiteRedis.h"
#include <iostream>
#include <string>
#include <sys/socket.h>  // Networking ke liye
#include <netinet/in.h>  // IP address structures ke liye
#include <unistd.h>      // close() function ke liye
#include <cstring>
#include <cstdlib>

#define PORT 6380 // Asli redis 6379 par chalta hai, hum 6380 use karenge

int main() {
    LiteRedis db(100); // 100 capacity ke sath database initialize kiya
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 1. Socket Create karna (Ek naya network endpoint banana)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Port ko forcefully attach karna taaki "Port already in use" error na aaye
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Kisi bhi IP se connection accept karo
    address.sin_port = htons(PORT);       // Port 6380

    // 3. Socket ko Port 6380 ke sath Bind (jodna) karna
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 4. Listen karna (Server ab incoming connections ka wait karega)
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "🚀 LiteRedis Server Started!\n";
    std::cout << "📡 Listening for connections on PORT " << PORT << "...\n";
    std::cout << "------------------------------------------------\n";

    // 5. Infinite Loop: Server hamesha on rahega aur clients ko accept karega
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        std::cout << "[SERVER] New client connected successfully!\n";

        // Client ko ek welcome message bhejna (Asli Redis style mein +PONG)
        const char* welcome_msg = "+PONG\n";
        send(new_socket, welcome_msg, strlen(welcome_msg), 0);
        
        // Message bhej kar connection close kar dena (Abhi ke liye)
        close(new_socket);
    }

    return 0;
}