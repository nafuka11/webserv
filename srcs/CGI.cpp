#include "CGI.hpp"
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

    argv_[0] = new char[command.size() + 1];
    std::char_traits<char>::copy(argv_[0], command.c_str(), command.size() + 1);
    argv_[1] = new char[path_.size() + 1];
    std::char_traits<char>::copy(argv_[1], path.c_str(), path.size() + 1);
    argv_[2] = NULL;
}

void CGI::setEnvp()
{
    envp_ = NULL;
}

void CGI::Execute()
{
    int rc = execve("/usr/bin/php", argv_, envp_);

    if (rc == -1)
    {
        throw SystemError("execve", errno);
    }
}
