#include "CGI.hpp"
#include <map>
#include <sstream>
#include <unistd.h>
#include "SystemError.hpp"

#include <iostream>//TODO: 後で消す

const std::map<std::string, std::string> CGI::EXEC_PATH = CGI::createExecutePath();
const std::map<std::string,std::string> CGI::EXEC_COMMAND = CGI::createExecuteCommand();


CGI::CGI(const HTTPRequest &request, const Uri &uri, const ServerConfig &config,
         const std::string &method, const std::string &ip)
    : request_(request), uri_(uri), config_(config)
{
    setPath(uri_.getLocalPath());
    setArgv(uri_.getLocalPath());
    setEnvp(ip, method);
}

CGI::~CGI()
{
    for (int i = 0; argv_[i]; i++)
    {
        delete argv_[i];
    }
    delete[] argv_;

    for (int i = 0; envp_[i]; i++)
    {
        delete envp_[i];
    }
    delete[]  envp_;
}

std::map<std::string,std::string> CGI::createExecutePath()
{
    std::map<std::string,std::string> exec_path;

    exec_path[".php"] = "/usr/bin/php";
    return exec_path;
}

std::map<std::string, std::string> CGI::createExecuteCommand()
{
    std::map<std::string,std::string> exec_command;

    exec_command[".php"] = "php";
    return exec_command;
}


void CGI::setPath(const std::string &local_path)
{
    size_t extension_pos = local_path.rfind(".");
    extension_ = local_path.substr(extension_pos);
    path_ = EXEC_PATH.find(extension_)->second;
}

void CGI::setArgv(const std::string &path)
{
    argv_ = new char*[3];
    std::string command = EXEC_COMMAND.find(extension_)->second;

    argv_[0] = new char[std::strlen(command.c_str()) + 1];
    std::char_traits<char>::copy(argv_[0], command.c_str(), std::strlen(command.c_str()) + 1);
    argv_[1] = new char[path.size() + 1];
    std::char_traits<char>::copy(argv_[1], path.c_str(), path.size() + 1);
    argv_[2] = NULL;
}

void CGI::setEnvp(const std::string &ip, const std::string &method)
{
    std::map<std::string, std::string> envs;
    std::map<std::string, std::string> headers = request_.getHeaders();

    std::map<std::string, std::string>::const_iterator authorization = headers.find("authorization");
    if (authorization != headers.end())
    {
        envs["AUTH_TYPE"] = authorization->second;
    }
    else
    {
        envs["AUTH_TYPE"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_length = headers.find("content-length");
    if (content_length != headers.end())
    {
        envs["CONTENT_LENGTH"] = content_length->second;
    }
    else
    {
        envs["CONTENT_LENGTH"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_type = headers.find("content-type");
    if (content_type != headers.end())
    {
        envs["CONTENT_TYPE"] = content_type->second;
    }
    envs["GATEWAY_INTERFACE"] = "CGI/1.1";
    envs["PATH_INFO"] = uri_.getRawPath();
    envs["PATH_TRANSLATED"] = "(test)";
    envs["QUERY_STRING"] = uri_.getQuery();
    envs["REMOTE_ADDR"] = ip;
    envs["REMOTE_HOST"] = "";
    envs["REMOTE_IDENT"] = "";
    envs["REMOTE_USER"] = "";
    envs["REQUEST_METHOD"] = method;
    envs["SCRIPT_NAME"] = envs["PATH_INFO"];
    envs["SERVER_NAME"] = headers["host"];

    std::stringstream ss;
    ss << config_.listen();
    envs["SERVER_PORT"] = ss.str();
    envs["SERVER_PROTOCOL"] = "HTTP/1.1";
    envs["SERVER_SOFTWARE"] = "webserv/1.0.0";

    envp_ = new char*[18];
    size_t index = 0;
    for (std::map<std::string, std::string>::const_iterator env = envs.begin();
         env != envs.end();
         env++, index++)
    {
        std::string env_str = env->first + "=" + env->second;
        envp_[index] = new char[env_str.size() + 1];
        std::char_traits<char>::copy(envp_[index], env_str.c_str(), env_str.size() + 1);
        // envp_[index] = strdup(env->c_str());
    }
    envp_[index] = NULL;
}

void CGI::Execute()
{
    int rc = execve(path_.c_str(), argv_, envp_);

    if (rc == -1)
    {
        throw SystemError("execve", errno);
    }
}
