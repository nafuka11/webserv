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
    std::string receive_request();
    void send_response(const std::string &message);
    void close();
};

#endif /* CLIENTSOCKET_HPP */
