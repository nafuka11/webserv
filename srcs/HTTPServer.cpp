#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "HTTPParseException.hpp"

HTTPServer::HTTPServer(int port)
{
    ServerSocket server(port);
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

HTTPServer::~HTTPServer()
{
}
