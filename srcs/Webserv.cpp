#include "Webserv.hpp"

Webserv::Webserv(const std::string &filepath) : config_(Config())
{
    // TODO: Configのファイル読み込み処理実装後、readFileに引数filepathをセットすること
    (void)filepath;
    config_.readFile();
    setServers();
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

void Webserv::setServers()
{
    for (size_t i = 0; i < config_.server().size(); i++)
    {
        int port = config_.server()[i].listen();
        HTTPServer http_server(port);
        servers_.push_back(http_server);
    }
}
