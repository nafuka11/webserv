#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>
#include "ServerConfig.hpp"

class Config
{
public:
    Config();
    ~Config();
    void readFile(const std::string &filepath);
    void addServer(const ServerConfig &server_config);
    const std::map<int, std::vector<ServerConfig> > &server() const;

private:
    std::map<int, std::vector<ServerConfig> > server_;
};

#endif /* CONFIG_HPP */
