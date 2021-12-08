#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <vector>
#include "Config.hpp"
#include "HTTPServer.hpp"

class Webserv
{
public:
    Webserv(const std::string &filepath);
    ~Webserv();
    void run();

private:
    Config config_;
    std::vector<HTTPServer> servers_;

    void setServers();
};

#endif /* WEBSERV_HPP */
