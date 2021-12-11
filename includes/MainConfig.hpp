#ifndef MAINCONFIG_HPP
#define MAINCONFIG_HPP

#include <map>
#include <string>
#include <vector>
#include "ServerConfig.hpp"

class ServerConfig;

class MainConfig
{
public:
    MainConfig();
    ~MainConfig();
    void addAllowMethod(const std::string &method);
    void setAutoIndex(const std::string &autoindex);
    void setClientMaxBodySize(const int size);
    void setcgiExtension(const std::string &extension);
    void addErrorPage(const int status_code, const std::string &uri);
    void addIndex(const std::string &file);
    void addServer(const ServerConfig &server_config);

    const std::vector<std::string> allowMethods() const;
    const std::string autoindex() const;
    const int &clientMaxBodySize() const;
    const std::string cgiExtension() const;
    const std::map<int, std::string> errorPage() const;
    const std::vector<std::string> index() const;
    const std::vector<ServerConfig> &server() const;
private:
    static const int DEFAULT_CLIENT_MAX_BODY_SIZE;

    std::vector<std::string> allow_method_;
    std::string autoindex_;
    int client_max_body_size_;
    std::string cgi_extension_;
    std::map<int, std::string> error_page_;
    std::vector<std::string> index_;
    std::vector<ServerConfig> server_;
};

#endif /* MAINCONFIG_HPP */
