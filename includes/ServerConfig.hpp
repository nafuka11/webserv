#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include <string>
#include <vector>

#include "LocationConfig.hpp"

class ServerConfig
{
public:
    ServerConfig();
    ~ServerConfig();

    void setAutoindex(const std::string &autoindex);
    void setCgiExtension(const std::string &extension);
    void setClientMaxBodySize(const int size);
    void setListen(const int port);
    void setServerName(const std::string &name);
    void setUploadPath(const std::string &path);
    void addAllowMethod(const std::string &method);
    void addErrorPage(const int status_code, const std::string &uri);
    void addIndex(const std::string &file);
    void addLocation(const std::string &path, const LocationConfig &location_config);
    void addReturnRedirect(const int status_code, const std::string &uri);

    void clearAllowMethod();
    void clearErrorPage(const int status_code);
    void clearIndex();
    void clearReturnRedirect(const int status_code);

    const std::vector<std::string> allowMethod() const;
    const std::string autoindex() const;
    const std::string cgiExtension() const;
    const int &clientMaxBodySize() const;
    const std::map<int, std::string> errorPage() const;
    const std::vector<std::string> index() const;
    const int &listen() const;
    const std::map<std::string, LocationConfig> location() const;
    const std::map<int, std::string> returnRedirect() const;
    const std::string serverName() const;
    const std::string uploadPath() const;

private:
    static const int DEFAULT_PORT;

    std::vector<std::string> allow_method_;
    std::string autoindex_;
    std::string cgi_extension_;
    int client_max_body_size_;
    std::map<int, std::string> error_page_;
    std::vector<std::string> index_;
    int listen_;
    std::map<std::string, LocationConfig> location_;
    std::map<int, std::string> return_redirect_;
    std::string server_name_;
    std::string upload_path_;
};

#endif /* SERVERCONFIG_HPP */
