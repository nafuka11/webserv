#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>

HTTPServer::HTTPServer(int port)
{
    ServerSocket server(port);
    std::string message = "Hello webserv!\n";
    while (true)
    {
        ClientSocket client = server.accept_connection();
        client.send(message);
        client.close();
    }
}

HTTPServer::~HTTPServer()
{
}
