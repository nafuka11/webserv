#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"
#include "ServerConfig.hpp"

class ServerSocket : public Socket
{
public:
    ServerSocket(const ServerConfig &config);
    ~ServerSocket();
    ClientSocket acceptConnection();

private:
    const ServerConfig &config_;

    void open();
    int openFromAddress(struct addrinfo *address);
    void listen();
};

#endif /* SERVERSOCKET_HPP */
