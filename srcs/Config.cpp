#include "Config.hpp"

#include <iostream>
Config::Config()
{
}

Config::Config(int listen1, int listen2)
{
	server.push_back(ServerConfig());
	server.push_back(ServerConfig());

	server[0].setListen(listen1);
	server[1].setListen(listen2);
}

Config::~Config()
{
}