#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"

class ClientSocket : public Socket
{
private:
    /* data */
public:
    ClientSocket(int fd, const struct sockaddr_storage &address);
    ~ClientSocket();
    std::string receiveRequest();
    void sendResponse(const std::string &message);
    void close();
};

#endif /* CLIENTSOCKET_HPP */
