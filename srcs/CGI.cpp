#include "CGI.hpp"
#include <map>
#include <sstream>
#include <unistd.h>
#include "SystemError.hpp"

const std::map<std::string, std::string> CGI::EXEC_PATH = CGI::createExecutePath();
const std::map<std::string,std::string> CGI::EXEC_COMMAND = CGI::createExecuteCommand();


CGI::CGI(const HTTPRequest &request, const Uri &uri, const ServerConfig &config,
         const std::string &method, const std::string &ip)
    : request_(request), uri_(uri), config_(config)
{
    setPath(uri_.getLocalPath());
    setArgs(uri_.getLocalPath());
    setEnvs(ip, method);
}

CGI::~CGI()
{
    for (int i = 0; exec_args_[i]; i++)
    {
        delete exec_args_[i];
    }
    delete[] exec_args_;

    for (int i = 0; exec_envs_[i]; i++)
    {
        delete exec_envs_[i];
    }
    delete[]  exec_envs_;
}

std::map<std::string,std::string> CGI::createExecutePath()
{
    std::map<std::string,std::string> path;

    path[".php"] = "/usr/bin/php";
    path[".pl"] =  "/usr/bin/perl";
    return path;
}

std::map<std::string, std::string> CGI::createExecuteCommand()
{
    std::map<std::string,std::string> command;

    command[".php"] = "php";
    command[".pl"] = "perl";
    return command;
}


void CGI::setPath(const std::string &local_path)
{
    size_t extension_pos = local_path.rfind(".");
    extension_ = local_path.substr(extension_pos);
    exec_path_ = EXEC_PATH.find(extension_)->second;
}

void CGI::setArgs(const std::string &path)
{
    std::vector<std::string> uri_args = uri_.getArguments();
    std::string command = EXEC_COMMAND.find(extension_)->second;
    int size = uri_args.size() + 3;
    int index = 0;

    exec_args_ = new char*[size];
    exec_args_[index++] = allocateString(command.c_str());
    exec_args_[index++] = allocateString(path.c_str());

    for (std::vector<std::string>::iterator arg = uri_args.begin();
         arg != uri_args.end();
         ++arg)
    {
        exec_args_[index++] = allocateString(arg->c_str());
    }
    exec_args_[index] = NULL;
}

void CGI::setEnvs(const std::string &ip, const std::string &method)
{
    std::map<std::string, std::string> tmp_envs;
    std::map<std::string, std::string> headers = request_.getHeaders();

    std::map<std::string, std::string>::const_iterator authorization = headers.find("authorization");
    if (authorization != headers.end())
    {
        tmp_envs["AUTH_TYPE"] = authorization->second;
    }
    else
    {
        tmp_envs["AUTH_TYPE"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_length = headers.find("content-length");
    if (content_length != headers.end())
    {
        tmp_envs["CONTENT_LENGTH"] = content_length->second;
    }
    else
    {
        tmp_envs["CONTENT_LENGTH"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_type = headers.find("content-type");
    if (content_type != headers.end())
    {
        tmp_envs["CONTENT_TYPE"] = content_type->second;
    }
    tmp_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
    tmp_envs["PATH_INFO"] = uri_.getRawPath();
    tmp_envs["PATH_TRANSLATED"] = "";
    tmp_envs["QUERY_STRING"] = uri_.getQuery();
    tmp_envs["REMOTE_ADDR"] = ip;
    tmp_envs["REMOTE_HOST"] = "";
    tmp_envs["REMOTE_IDENT"] = "";
    tmp_envs["REMOTE_USER"] = "";
    tmp_envs["REQUEST_METHOD"] = method;
    tmp_envs["SCRIPT_NAME"] = tmp_envs["PATH_INFO"];
    tmp_envs["SERVER_NAME"] = headers["host"];

    std::stringstream ss;
    ss << config_.listen();
    tmp_envs["SERVER_PORT"] = ss.str();
    tmp_envs["SERVER_PROTOCOL"] = "HTTP/1.1";
    tmp_envs["SERVER_SOFTWARE"] = "webserv/1.0.0";

    exec_envs_ = new char*[18];
    size_t index = 0;
    for (std::map<std::string, std::string>::const_iterator env = tmp_envs.begin();
         env != tmp_envs.end();
         env++, index++)
    {
        std::string env_str = env->first + "=" + env->second;
        exec_envs_[index] = allocateString(env_str.c_str());
    }
    exec_envs_[index] = NULL;
}

char *CGI::allocateString(const std::string &str)
{
    char *allocated = strdup(str.c_str());

    if (allocated == NULL)
    {
        throw SystemError("strdup", errno);
    }
    return allocated;
}

void CGI::execute()
{
    int rc = execve(exec_path_.c_str(), exec_args_, exec_envs_);

    if (rc == -1)
    {
        throw SystemError("execve", errno);
    }
}
