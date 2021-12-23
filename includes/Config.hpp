#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConfig.hpp"

class Config
{
public:
    Config();
    ~Config();
    void readFile(const std::string &filepath);
    void addServer(const ServerConfig &server_config);
    const std::vector<ServerConfig> &server() const;
private:
    std::vector<ServerConfig> server_;
};

#endif /* CONFIG_HPP */
