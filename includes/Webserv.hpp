#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
#include <vector>
#include "Config.hpp"
#include "Socket.hpp"
#include "ClientSocket.hpp"
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
    void handleServerEvent(Socket *socket, const struct kevent &event);
    void handleClientEvent(Socket *socket, const struct kevent &event);
    void closeClient(ClientSocket *client);
};

#endif /* WEBSERV_HPP */
