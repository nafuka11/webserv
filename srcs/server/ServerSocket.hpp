#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"
#include "ServerConfig.hpp"
#include "KqueuePoller.hpp"

class ServerSocket : public Socket
{
public:
    ServerSocket(int port, const std::vector<ServerConfig> &configs);
    ~ServerSocket();
    ClientSocket *acceptConnection(KqueuePoller &poller) const;

private:
    const std::vector<ServerConfig> &configs_;

    void open(int port);
    int openFromAddress(struct addrinfo *address);
    void listen();
};

#endif /* SERVERSOCKET_HPP */
