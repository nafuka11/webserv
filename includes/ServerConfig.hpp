#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <map>
#include <string>

class ServerConfig
{
public:
    ServerConfig();
    ~ServerConfig();
    void setListen(const int listen);
    void setServerName(const std::string name);
    void addAllowMethods(const std::string method);
    void setcgiExtension(const std::string extension);
    void setClientMaxBodySize(const int size);
    void addErrorPage(const int code, const std::string uri);
    void setUploadPath(const std::string path);
    void addLocation(const std::string path, const LocationConfig location_config);
    const int listen() const;
    const std::string serverName() const;
    const std::vector<std::string> allowMethods() const;
    const std::string cgiExtension() const;
    const int clientMaxBodySize() const;
    const std::map<int, std::string> errorPage() const;
    const std::string uploadPath() const;
    const std::map<std::string, LocationConfig> location();
private:
    int listen_;
    std::string server_name_;
    std::vector<std::string> allow_methods_;
    std::string cgi_extension_;
    int client_max_body_size_;
    std::map<int, std::string> error_page_;
    std::string upload_path_;
    std::map<std::string, LocationConfig> location_;
};

#endif /* SERVERCONFIG_HPP */
