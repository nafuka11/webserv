#ifndef MAINCONFIG_HPP
#define MAINCONFIG_HPP

#include <map>
#include <string>
#include <vector>
#include "ServerConfig.hpp"

class MainConfig
{
public:
    MainConfig();
    ~MainConfig();
    void setAutoindex(const std::string &autoindex);
    void setClientMaxBodySize(const int size);
    void addAllowMethod(const std::string &method);
    void addCgiExtensions(const std::string &extension);
    void addErrorPage(const int status_code, const std::string &uri);
    void addIndex(const std::string &file);
    void addServer(const ServerConfig &server_config);

    void clearErrorPage(int status_code);
    void clearIndex();

    const std::vector<std::string> allowMethod() const;
    const std::string autoindex() const;
    const int &clientMaxBodySize() const;
    const std::vector<std::string> &cgiExtensions() const;
    const std::map<int, std::string> errorPage() const;
    const std::vector<std::string> index() const;
    const std::vector<ServerConfig> &server() const;
private:
    std::vector<std::string> allow_method_;
    std::string autoindex_;
    std::vector<std::string> cgi_extensions_;
    int client_max_body_size_;
    std::map<int, std::string> error_page_;
    std::vector<std::string> index_;
    std::vector<ServerConfig> server_;
};

#endif /* MAINCONFIG_HPP */
