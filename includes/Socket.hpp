#ifndef SOCKET_HPP
#define SOCKET_HPP

class Socket
{
protected:
    int port;
    int fd;

public:
    Socket(int port);
    virtual ~Socket();
};

#endif /* SOCKET_HPP */
