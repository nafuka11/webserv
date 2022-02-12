#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include "HTTPRequest.hpp"
#include "ServerConfig.hpp"
#include "Uri.hpp"

class CGI
{
public:
    CGI();
    ~CGI();
    void run(const HTTPRequest &request,  const ServerConfig &config,
             const std::string &ip, const std::string &method, const Uri &uri);
    void end();
    int getFdWriteToCGI() const;
    int getFdReadFromCGI() const;
    pid_t getChildPID() const;

private:
    HTTPRequest request_;
    ServerConfig config_;
    std::string method_;
    std::string extension_;
    std::string exec_path_;
    std::string local_path_;
    std::string ip_;
    char **exec_args_;
    char **exec_envs_;
    int pipe_cgi_read_[2];
    int pipe_cgi_write_[2];
    pid_t child_pid_;

    static const std::map<std::string,std::string> EXEC_PATH;
    static const std::map<std::string,std::string> EXEC_COMMAND;
    static std::map<std::string, std::string> createExecutePath();
    static std::map<std::string, std::string> createExecuteCommand();

    void createPipe();
    pid_t spawnChild();
    void prepareCGIInOut();
    void prepareServerInOut();

    void setMembersValue(const HTTPRequest &request, const ServerConfig &config,
                         const std::string &ip, const std::string &method, const Uri &uri);
    void setPath();
    void setArgs(const Uri &uri);
    void setEnvs(const Uri &uri);

    char *allocateString(const std::string &str);
    void close(int fd);
    void duplicateFd(int oldfd, int newfd);
    void execve();
    void deleteAllocated();
};

#endif /* CGI_HPP */
