#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"

class ServerSocket : public Socket
{
private:
    void open();
    int open_from_address(struct addrinfo *address);
    void listen();

public:
    ServerSocket(int port);
    ~ServerSocket();
    int accept_connection();
};

#endif /* SERVERSOCKET_HPP */
