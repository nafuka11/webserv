#include "ClientSocket.hpp"
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include <cerrno>
#include <unistd.h>

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address)
    : Socket(fd), parser_(request_), state_(READ)
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
    {
        state_ = CLOSE;
        request_.clear();
        parser_.clear();
        return;
    }
    buffer[read_byte] = '\0';
    parser_.appendRawMessage(buffer);
    try
    {
        parser_.parse();
        if (parser_.finished())
        {
            state_ = WRITE;
            response_.setStatusCode(CODE_200);
        }
    }
    catch(const HTTPParseException &e)
    {
        state_ = WRITE;
        response_.setStatusCode(e.getStatusCode());
    }
}

void ClientSocket::sendResponse()
{
    std::string message = response_.toString();
    ::send(fd_, message.c_str(), message.size(), 0);
    if (request_.canKeepAlive())
    {
        state_ = READ;
    }
    else
    {
        state_ = CLOSE;
    }
    request_.clear();
    parser_.clear();
}

void ClientSocket::close()
{
    if (::close(fd_) < 0)
    {
        throw SystemError("close", errno);
    }
}

ClientSocket::State ClientSocket::getState() const
{
    return state_;
}
