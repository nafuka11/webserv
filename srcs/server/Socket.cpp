#include "Socket.hpp"
#include <cerrno>
#include <fcntl.h>
#include "SystemError.hpp"

Socket::Socket(Type type) : type_(type)
{
}

Socket::Socket(Type type, int fd) : type_(type), fd_(fd)
{
}

Socket::~Socket()
{
}

Socket::Type Socket::getType() const
{
    return type_;
}

int Socket::getFd() const
{
    return fd_;
}

void Socket::setNonBlockingFd(int fd) const
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
    {
        throw SystemError("fcntl", errno);
    }
}
