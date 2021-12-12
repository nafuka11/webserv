#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>

class Socket
{
public:
    enum Type
    {
        SERVER,
        CLIENT
    };

    Socket(Type type);
    Socket(Type type, int fd);
    virtual ~Socket();

    Type getType() const;
    int getFd() const;

protected:
    Type type_;
    int fd_;
    struct sockaddr_storage address_;
};

#endif /* SOCKET_HPP */
