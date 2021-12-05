#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : listen_()
{
}

ServerConfig::~ServerConfig()
{
}

void	ServerConfig::setListen(const int port)
{
	listen_ = port;
}

const int	ServerConfig::listen() const
{
	return listen_;
}
