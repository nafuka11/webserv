#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPParser.hpp"
#include "ServerConfig.hpp"

class ClientSocket : public Socket
{
public:
    enum State
    {
        READ_REQUEST,
        WRITE_RESPONSE,
        CLOSE
    };

    ClientSocket(int fd, const struct sockaddr_storage &address,
                 const ServerConfig &config_);
    ~ClientSocket();
    void receiveRequest();
    void sendResponse();
    void close();
    State getState() const;

private:
    static const size_t BUF_SIZE;
    const ServerConfig &config_;
    HTTPRequest request_;
    HTTPResponse response_;
    HTTPParser parser_;
    State state_;

    void clearRequest();
};

#endif /* CLIENTSOCKET_HPP */
