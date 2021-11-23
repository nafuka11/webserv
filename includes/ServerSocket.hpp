#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"

class ServerSocket : public Socket
{
private:
    void open();
    int open_from_address(struct addrinfo *address);
    void listen();

public:
    ServerSocket(int port);
    ~ServerSocket();
    ClientSocket accept_connection();
};

#endif /* SERVERSOCKET_HPP */
