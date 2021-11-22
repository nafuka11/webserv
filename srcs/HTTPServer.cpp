#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>

HTTPServer::HTTPServer(int port)
{
    ServerSocket socket(port);
    std::string message = "Hello webserv!\n";
    while (true)
    {
        int connect_d = socket.accept_connection();
        send(connect_d, message.c_str(), message.size(), 0);
        close(connect_d);
    }
}

HTTPServer::~HTTPServer()
{
}
