#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPParser.hpp"

class ClientSocket : public Socket
{
public:
    enum State {
        READ,
        WRITE,
        CLOSE
    };

    ClientSocket(int fd, const struct sockaddr_storage &address);
    ~ClientSocket();
    void receiveRequest();
    void sendResponse();
    void close();
    State getState() const;

private:
    static const size_t BUF_SIZE;
    std::string message_;
    HTTPRequest request_;
    HTTPResponse response_;
    HTTPParser parser_;
    State state_;
};

#endif /* CLIENTSOCKET_HPP */
