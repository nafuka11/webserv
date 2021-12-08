#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "HTTPParseException.hpp"

HTTPServer::HTTPServer(const ServerConfig &config) : config_(config)
{
}

HTTPServer::~HTTPServer()
{
}

void HTTPServer::run()
{
    ServerSocket server(config_.listen());
    while (true)
    {
        ClientSocket client = server.acceptConnection();
        while (client.getState() != ClientSocket::CLOSE)
        {
            switch (client.getState())
            {
            case ClientSocket::READ:
                client.receiveRequest();
                break;
            case ClientSocket::WRITE:
                client.sendResponse();
                break;
            default:
                break;
            }
        }
        client.close();
    }
}
