#include "HTTPServer.hpp"
#include "ServerSocket.hpp"

#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "HTTPParseException.hpp"

HTTPServer::HTTPServer(int port)
{
    ServerSocket server(port);
    while (true)
    {
        ClientSocket client = server.acceptConnection();
        while (true)
        {
            try
            {
                client.receiveRequest();
            }
            catch(const HTTPParseException& e)
            {
                std::cerr << "HTTP Parse Error: code: " << e.getStatusCode() << std::endl;
            }
            // client.sendResponse(request);
        }
        client.close();
    }
}

HTTPServer::~HTTPServer()
{
}
