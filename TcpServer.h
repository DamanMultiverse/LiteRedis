#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "LiteRedis.h"
#include <string>

class TcpServer {
private:
    int server_fd;
    LiteRedis& db;
    bool is_running;

    static void handleClient(int client_socket, LiteRedis& db);

public:
    TcpServer(LiteRedis& database);
    ~TcpServer();

    void start();
    void stop();
};

#endif // TCPSERVER_H