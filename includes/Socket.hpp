#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>

class Socket
{
protected:
    int port_;
    int fd_;
    struct sockaddr_storage address_;

public:
    Socket();
    Socket(int fd);
    virtual ~Socket();
};

#endif /* SOCKET_HPP */
