#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPParser.hpp"

class ClientSocket : public Socket
{
private:
    static const size_t BUF_SIZE;
    std::string message_;
    HTTPRequest request_;
    HTTPParser parser_;

public:
    ClientSocket(int fd, const struct sockaddr_storage &address);
    ~ClientSocket();
    void receiveRequest();
    void sendResponse(const std::string &message);
    void close();
};

#endif /* CLIENTSOCKET_HPP */
