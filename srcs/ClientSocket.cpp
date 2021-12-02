#include "ClientSocket.hpp"
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include <cerrno>
#include <unistd.h>

#include <iostream>

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address)
    : Socket(fd), parser_(request_)
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
    parser_.appendRawMessage(buffer);
    try
    {
        parser_.parse();
        if (parser_.finished())
        {
            // TODO: 後で消しましょう。ログクラスを作って出力するのがいいかも
            std::cout << "method: " << request_.getMethod() << std::endl
            << "uri: \"" << request_.getUri() << "\"" << std::endl
            << "protocol: \"" << request_.getProtocolVersion() << "\"" << std::endl;
            const std::map<std::string, std::string> headers = request_.getHeaders();
            for (std::map<std::string, std::string>::const_iterator iter = headers.begin();
                iter != headers.end();
                ++iter)
            {
                std::cout << "\"" << iter->first << "\": \"" << iter->second << "\"" << std::endl;
            }
            // TODO: HTTPリクエストを作成しsendする。
            parser_.clear();
            request_.clear();
        }
    }
    catch(const HTTPParseException &e)
    {
        parser_.clear();
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
