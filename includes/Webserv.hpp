#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <vector>
#include "Config.hpp"
#include "Socket.hpp"
#include "KqueuePoller.hpp"

class Webserv
{
public:
    Webserv(const std::string &filepath);
    ~Webserv();
    void run();

private:
    Config config_;
    KqueuePoller poller_;
    std::map<int, Socket *> sockets_;

    void setupServers();
    void registerEvents();
    void watchEvents();
};

#endif /* WEBSERV_HPP */
