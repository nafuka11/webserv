#include "ClientSocket.hpp"
#include "SystemError.hpp"
#include <cerrno>
#include <unistd.h>

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address)
    : Socket(fd)
{
    address_ = address;
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::send(const std::string &message)
{
    ::send(fd_, message.c_str(), message.size(), 0);
}

void ClientSocket::close()
{
    if (::close(fd_) < 0)
    {
        throw SystemError("close", errno);
    }
}
