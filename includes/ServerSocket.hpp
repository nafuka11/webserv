#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"

class ServerSocket : public Socket
{
private:
    void open();
    int openFromAddress(struct addrinfo *address);
    void listen();

public:
    ServerSocket(int port);
    ~ServerSocket();
    ClientSocket acceptConnection();
};

#endif /* SERVERSOCKET_HPP */
