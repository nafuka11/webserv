#include "ClientSocket.hpp"
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include <cerrno>
#include <unistd.h>

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address)
    : Socket(fd)
{
    address_ = address;
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::receiveRequest()
{
    char buffer[BUF_SIZE];
    int read_byte = recv(fd_, buffer, BUF_SIZE - 1, 0);
    if (read_byte <= 0)
        return;
    buffer[read_byte] = '\0';
    request_.appendRawMessage(buffer);
    try
    {
        request_.parse();
    }
    catch(const HTTPParseException &e)
    {
        request_.clear();
        throw e;
    }
}

void ClientSocket::sendResponse(const std::string &message)
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
