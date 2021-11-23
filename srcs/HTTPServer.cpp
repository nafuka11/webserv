#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

HTTPServer::HTTPServer(int port)
{
    ServerSocket server(port);
    std::string message = "Hello webserv!\n";
    while (true)
    {
        ClientSocket client = server.accept_connection();
        while (true)
        {
            std::string request = client.receive_request();
            std::cout << "Received: " << request.size() << " byte: " << std::endl
                      << request << std::endl;
            if (request.size() <= 2)
                break;
            client.send_response(request);
        }
        client.close();
    }
}

HTTPServer::~HTTPServer()
{
}
