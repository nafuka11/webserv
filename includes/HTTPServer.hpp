#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "ServerConfig.hpp"

class HTTPServer
{
public:
    HTTPServer(const ServerConfig &config);
    ~HTTPServer();
    void run();

private:
    const ServerConfig &config_;
};

#endif /* HTTPSERVER_HPP */
