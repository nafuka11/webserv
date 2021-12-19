#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <map>
#include <string>
#include <vector>

class ServerConfig;

class LocationConfig
{
public:
    LocationConfig(const ServerConfig &server_config);
    ~LocationConfig();

    void setAlias(const std::string &path);
    void addAllowMethod(const std::string &method);
    void setAutoindex(const std::string &autoindex);
    void addErrorPage(const int status_code, const std::string &uri);
    void addIndex(const std::string &file);
    void addReturnRedirect(const int status_code, const std::string &uri);
    void setUploadPath(const std::string &path);

    void clearAllowMethod();
    void clearErrorPage(int status_code);
    void clearIndex();
    void clearReturnRedirect();

    const std::string alias() const;
    const std::vector<std::string>allowMethod() const;
    const std::string autoindex() const;
    const std::map<int, std::string> errorPage() const;
    const std::vector<std::string> index() const;
    const std::map<int, std::string> returnRedirect() const;
    const std::string uploadPath() const;
private:
    std::string alias_;
    std::vector<std::string> allow_method_;
    std::string autoindex_;
    std::map<int, std::string> error_page_;
    std::vector<std::string> index_;
    std::map<int, std::string> return_;
    std::string upload_path_;
};

#endif /* LOCATIONCONFIG_HPP */
