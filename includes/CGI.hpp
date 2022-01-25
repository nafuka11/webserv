#ifndef CGI_HPP
#define CGI_HPP

#include <string>

class CGI
{
public:
    CGI(const std::string &path);
    ~CGI();
    void Execute();
private:
    void setPath(const std::string &path);
    void setArgv(const std::string &path);
    void setEnvp();

    std::string path_;
    char **argv_;
    char **envp_;
};

#endif /* CGI_HPP */
