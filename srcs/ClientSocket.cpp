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

std::string ClientSocket::receive_request()
{
    size_t buffer_size = 4096;
    char buffer[buffer_size];
    int read_byte = recv(fd_, buffer, buffer_size - 1, 0);
    if (read_byte <= 0)
        return "";
    buffer[read_byte] = '\0';
    return std::string(buffer);
}

void ClientSocket::send_response(const std::string &message)
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
