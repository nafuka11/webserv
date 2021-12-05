#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

#include "ServerConfig.hpp"

class Config
{
public:
	Config();
	Config(int listen1, int listen2); //del
	~Config();
	std::vector<ServerConfig> server;
private:
};

#endif /* CONFIG_HPP */