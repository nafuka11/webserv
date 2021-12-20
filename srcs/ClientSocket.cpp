#include "ClientSocket.hpp"
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include <cerrno>
#include <unistd.h>

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address,
                           const ServerConfig &config, const KqueuePoller &poller)
    : Socket(CLIENT, fd), config_(config), poller_(poller),
      parser_(request_, config_), state_(READ_REQUEST)
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
        clearRequest();
        return;
    }
    buffer[read_byte] = '\0';
    parser_.appendRawMessage(buffer);
    try
    {
        parser_.parse();
        if (parser_.finished())
        {
            state_ = WRITE_RESPONSE;
            response_.setStatusCode(CODE_200);
            poller_.registerWriteEvent(this, fd_);
        }
    }
    catch (const HTTPParseException &e)
    {
        state_ = WRITE_RESPONSE;
        response_.setStatusCode(e.getStatusCode());
        poller_.registerWriteEvent(this, fd_);
    }
}

void ClientSocket::sendResponse()
{
    response_.setKeepAlive(request_.canKeepAlive());
    std::string message = response_.toString();
    ::send(fd_, message.c_str(), message.size(), 0);
    if (request_.canKeepAlive())
    {
        state_ = READ_REQUEST;
        poller_.unregisterWriteEvent(this, fd_);
        poller_.registerReadEvent(this, fd_);
    }
    else
    {
        state_ = CLOSE;
    }
    clearRequest();
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

void ClientSocket::clearRequest()
{
    request_.clear();
    parser_.clear();
}
