#include "CGI.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>
#include "SystemError.hpp"

const std::map<std::string, std::string> CGI::EXEC_PATH = CGI::createExecutePath();
const std::map<std::string,std::string> CGI::EXEC_COMMAND = CGI::createExecuteCommand();

CGI::CGI()
{
}

CGI::~CGI()
{
}

void CGI::run(const HTTPRequest &request,  const ServerConfig &config,
                        const std::string &ip, const std::string &method, const Uri &uri)
{
    request_ = request;
    config_ = config;
    ip_ = ip;
    method_= method;
    local_path_ = uri.getLocalPath();

    setPath();
    setArgs(uri);
    setEnvs(uri);
    createPipe();
    child_pid_ = spawnChild();
}

void CGI::end()
{
    if (method_  == "POST")
    {
        close(pipe_cgi_read_[1]);
    }
    close(pipe_cgi_write_[0]);
    deleteAllocated();
}

pid_t CGI::spawnChild()
{
    pid_t pid = fork();
    if (pid < 0)
    {
        throw SystemError("fork", errno);
    }
    if (pid == 0)
    {
        try
        {
            prepareCGIInOut();
            execve();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        prepareServerInOut();
        return pid;
    }
}

int CGI::getFdWriteToCGI() const
{
    return pipe_cgi_read_[1];
}

int CGI::getFdReadFromCGI() const
{
    return pipe_cgi_write_[0];
}

pid_t CGI::getChildPID() const
{
    return child_pid_;
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

void CGI::createPipe()
{
    if (method_ == "POST")
    {
        if (pipe(pipe_cgi_read_) < 0)
        {
            throw SystemError("pipe", errno);
        }
    }
    if (pipe(pipe_cgi_write_) < 0)
    {
        throw SystemError("pipe", errno);
    }
}

void CGI::prepareCGIInOut()
{
    if (method_ == "POST")
    {
        close(pipe_cgi_read_[1]);
        close(STDIN_FILENO);
        duplicateFd(pipe_cgi_read_[0], STDIN_FILENO);
    }
    close(pipe_cgi_write_[0]);
    close(STDOUT_FILENO);
    duplicateFd(pipe_cgi_write_[1], STDOUT_FILENO);
}

void CGI::prepareServerInOut()
{
    if (method_ == "POST")
    {
        close(pipe_cgi_read_[0]);
    }
    close(pipe_cgi_write_[1]);
}

void CGI::setPath()
{
    size_t extension_pos = local_path_.rfind(".");
    extension_ = local_path_.substr(extension_pos);
    exec_path_ = EXEC_PATH.find(extension_)->second;
}

void CGI::setArgs(const Uri &uri)
{
    std::cerr << "Called setArgs(new)" << std::endl; //TODO: 後で消す
    std::vector<std::string> uri_args = uri.getArguments();
    std::string command = EXEC_COMMAND.find(extension_)->second;
    int size = uri_args.size() + 3;
    int index = 0;

    exec_args_ = new char*[size];
    exec_args_[index++] = allocateString(command);
    exec_args_[index++] = allocateString(local_path_);

    for (std::vector<std::string>::iterator arg = uri_args.begin();
         arg != uri_args.end();
         ++arg)
    {
        exec_args_[index++] = allocateString(*arg);
    }
    exec_args_[index] = NULL;
}

void CGI::setEnvs(const Uri &uri)
{
    std::map<std::string, std::string> tmp_envs;
    std::map<std::string, std::string> headers = request_.getHeaders();

    std::map<std::string, std::string>::const_iterator authorization =
        headers.find("authorization");
    if (authorization != headers.end())
    {
        tmp_envs["AUTH_TYPE"] = authorization->second;
    }
    else
    {
        tmp_envs["AUTH_TYPE"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_length =
        headers.find("content-length");
    if (content_length != headers.end())
    {
        tmp_envs["CONTENT_LENGTH"] = content_length->second;
    }
    else
    {
        tmp_envs["CONTENT_LENGTH"] = "";
    }
    std::map<std::string, std::string>::const_iterator content_type =
        headers.find("content-type");
    if (content_type != headers.end())
    {
        tmp_envs["CONTENT_TYPE"] = content_type->second;
    }
    tmp_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
    tmp_envs["PATH_INFO"] = uri.getRawPath();
    tmp_envs["PATH_TRANSLATED"] = "";
    tmp_envs["QUERY_STRING"] = uri.getQuery();
    tmp_envs["REMOTE_ADDR"] = ip_;
    tmp_envs["REMOTE_HOST"] = "";
    tmp_envs["REMOTE_IDENT"] = "";
    tmp_envs["REMOTE_USER"] = "";
    tmp_envs["REQUEST_METHOD"] = method_;
    tmp_envs["SCRIPT_NAME"] = tmp_envs["PATH_INFO"];
    tmp_envs["SERVER_NAME"] = headers["host"];

    std::stringstream ss;
    ss << config_.listen();
    tmp_envs["SERVER_PORT"] = ss.str();
    tmp_envs["SERVER_PROTOCOL"] = "HTTP/1.1";
    tmp_envs["SERVER_SOFTWARE"] = "webserv/1.0.0";

    exec_envs_ = new char*[tmp_envs.size() + 1];
    size_t index = 0;
    for (std::map<std::string, std::string>::const_iterator env = tmp_envs.begin();
         env != tmp_envs.end();
         env++, index++)
    {
        std::string env_str = env->first + "=" + env->second;
        exec_envs_[index] = allocateString(env_str);
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

void CGI::close(int fd)
{
    if (::close(fd) < 0)
    {
        throw SystemError("close",  errno);
    }
}

void CGI::duplicateFd(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) < 0)
    {
        throw SystemError("dup2", errno);
    }
}

void CGI::execve()
{
    int rc = ::execve(exec_path_.c_str(), exec_args_, exec_envs_);

    if (rc == -1)
    {
        throw SystemError("execve", errno);
    }
}

void CGI::deleteAllocated()
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
