//
// Created by victoria zubieta on 22/05/2026.
//

#ifndef TALLER_TP_SERVER__H
#define TALLER_TP_SERVER__H
#include "../common/socket/socket.h"

class Server {
private:
    Socket skt;
public:
    explicit Server(const char* servname);
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    void run();

};

#endif

