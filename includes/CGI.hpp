#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include "HTTPRequest.hpp"
#include "ServerConfig.hpp"
#include "Uri.hpp"

class CGI
{
public:
    CGI(const HTTPRequest &request,  const Uri &uri, const ServerConfig &config,
        const std::string &method, const std::string &ip);
    ~CGI();
    void Execute();
private:
    static const std::map<std::string,std::string> EXEC_PATH;
    static const std::map<std::string,std::string> EXEC_COMMAND;

    static std::map<std::string, std::string> createExecutePath();
    static std::map<std::string, std::string> createExecuteCommand();
    void setPath(const std::string &local_path);
    void setArgv(const std::string &path);
    void setEnvp(const std::string &ip, const std::string &method);

    HTTPRequest request_;
    Uri         uri_;
    ServerConfig config_;
    std::string extension_;
    std::string path_; // TODO: 必要なければ消す。
    char **argv_;
    char **envp_;
};

#endif /* CGI_HPP */
