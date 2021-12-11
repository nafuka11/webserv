#include "Webserv.hpp"
#include "ServerSocket.hpp"

Webserv::Webserv(const std::string &filepath) : config_(Config())
{
    // TODO: Configのファイル読み込み処理実装後、readFileに引数filepathをセットすること
    (void)filepath;
    config_.readFile();
    setupServers();
    registerEvents();
}

Webserv::~Webserv()
{
}

void Webserv::run()
{
    while (true)
    {
        watchEvents();
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

void Webserv::registerEvents()
{
    for (std::map<int, Socket *>::iterator iter = sockets_.begin();
         iter != sockets_.end(); ++iter)
    {
        poller_.registerServerSocket(iter->second);
    }
}

void Webserv::watchEvents()
{
}
