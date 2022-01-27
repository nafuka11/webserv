#include "CGI.hpp"
#include <vector>
#include <unistd.h>
#include "SystemError.hpp"

#include <iostream>//TODO: 後で消す

CGI::CGI(const std::string &path)
{
    setPath(path);
    setArgv(path);
    setEnvp();
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

void CGI::setPath(const std::string &path)
{
    (void)path;
    // TODO: pathの拡張子を元に、pathを設置する。(.php, .py...)
    path_ = "php";
}

void CGI::setArgv(const std::string &path)
{
    argv_ = new char*[3];
    std::string command = "php";

    argv_[0] = new char[std::strlen("php") + 1];
    std::char_traits<char>::copy(argv_[0], "php", std::strlen("php") + 1);
    argv_[1] = new char[path.size() + 1];
    std::char_traits<char>::copy(argv_[1], path.c_str(), path.size() + 1);
    argv_[2] = NULL;
}

void CGI::setEnvp()
{
    std::vector<std::string> envs;
    envs.push_back(std::string("AUTH_TYPE=") + std::string("test"));
    envs.push_back(std::string("CONTENT_LENGTH=") + std::string("test"));
    envs.push_back(std::string("CONTENT_TYPE=") + std::string("test"));
    envs.push_back(std::string("GATEWAY_INTERFACE=") + std::string("test"));
    envs.push_back(std::string("PATH_INFO=") + std::string("test"));
    envs.push_back(std::string("PATH_TRANSLATED=") + std::string("test"));
    envs.push_back(std::string("QUERY_STRING=") + std::string("test"));
    envs.push_back(std::string("REMOTE_ADDR=") + std::string("test"));
    envs.push_back(std::string("REMOTE_HOST=") + std::string("test"));
    envs.push_back(std::string("REMOTE_IDENT=") + std::string("test"));
    envs.push_back(std::string("REMOTE_USER=") + std::string("test"));
    envs.push_back(std::string("REQUEST_METHOD=") + std::string("test"));
    envs.push_back(std::string("SCRIP_NAME=") + std::string("test"));
    envs.push_back(std::string("SERVER_NAME=") + std::string("test"));
    envs.push_back(std::string("SERVER_PORT=") + std::string("test"));
    envs.push_back(std::string("SERVER_PROTOCOL=") + std::string("test"));
    envs.push_back(std::string("SERVER_SOFTWARE=") + std::string("test"));

    envp_ = new char*[18];
    size_t index = 0;
    for (std::vector<std::string>::const_iterator env = envs.begin();
         env != envs.end();
         env++, index++)
    {
        envp_[index] = new char[env->size() + 1];
        std::char_traits<char>::copy(envp_[index], env->c_str(), env->size() + 1);
    }
    envp_[index] = NULL;
}

void CGI::Execute()
{
    int rc = execve("/usr/bin/php", argv_, envp_);

    if (rc == -1)
    {
        throw SystemError("execve", errno);
    }
}
