#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConfig.hpp"

class Config
{
public:
    Config();
    ~Config();
    const std::vector<ServerConfig> getServer() const;
private:
    std::vector<ServerConfig> server_;
};

#endif /* CONFIG_HPP */