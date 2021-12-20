#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include "Socket.hpp"
#include "ClientSocket.hpp"
#include "ServerConfig.hpp"
#include "KqueuePoller.hpp"

class ServerSocket : public Socket
{
public:
    ServerSocket(const ServerConfig &config);
    ~ServerSocket();
    ClientSocket *acceptConnection(const KqueuePoller &poller) const;

private:
    const ServerConfig &config_;

    void open();
    int openFromAddress(struct addrinfo *address);
    void listen();
    void setNonBlockingFd(int fd) const;
};

#endif /* SERVERSOCKET_HPP */
