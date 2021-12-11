#include "Webserv.hpp"
#include "ServerSocket.hpp"

Webserv::Webserv(const std::string &filepath) : config_(Config())
{
    // TODO: Configのファイル読み込み処理実装後、readFileに引数filepathをセットすること
    (void)filepath;
    config_.readFile();
    setupServers();
}

Webserv::~Webserv()
{
}

void Webserv::run()
{
    for (size_t i = 0; i < servers_.size(); i++)
    {
        servers_[i].run();
    }
}

void Webserv::setupServers()
{
    for (size_t i = 0; i < config_.server().size(); i++)
    {
        ServerSocket *server = new ServerSocket(config_.server()[i]);
        sockets_.insert(std::make_pair(server->getFd(), server));
    }
}
