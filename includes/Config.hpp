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

#ifdef DEBUG
    void printServerConfig() const;
    void printServerName(const std::string &server_name) const;
    void printListen(const int listen) const;
    void printLocation(const std::map<std::string, LocationConfig> &location) const;
    void printAlias(const std::string &alias, const std::string &indent) const;
    void printAllowMethod(const std::vector<std::string> &allow_method,
                          const std::string &indent) const;
    void printAutoindex(const std::string &autoindex,
                        const std::string &indent) const;
    void printCgiExtension(const std::vector<std::string> &cgi_extension,
                           const std::string &indent) const;
    void printClientMaxBodySize(const int client_max_body_size,
                                const std::string &indent) const;
    void printErrorPage(const std::map<int, std::string> &error_page,
                        const std::string &indent) const;
    void printIndex(const std::vector<std::string> &index,
                    const std::string &indent) const;
    void printReturnRedirect(const std::map<int, std::string> &return_redirect,
                    const std::string &indent) const;
    void printUploadPath(const std::string &upload_path, const std::string &indent) const;
#endif
};

#endif /* CONFIG_HPP */
