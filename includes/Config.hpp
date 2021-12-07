#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include "ServerConfig.hpp"

class ConfigParser;

class Config
{
public:
    Config();
    ~Config();
    void readFile(const std::string &file_path); // 後で引数にファイルパスとして const std::string & をセットする。
    const std::vector<ServerConfig> server() const;
private:
    void addServer(const ServerConfig &server_config);
    std::vector<ServerConfig> server_;
};

#endif /* CONFIG_HPP */
