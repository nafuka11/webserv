#include "Socket.hpp"

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
