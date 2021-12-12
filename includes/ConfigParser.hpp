#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
public:
    enum ConfState
    {
        CONF_CONTEXT_MAIN,
        CONF_CONTEXT_SERVER,
        CONF_CONTEXT_LOCATION,
        CONF_DONE
    };

    ConfigParser(Config &config);
    ~ConfigParser();
    void readFile(const std::string &filepath);
private:
    class ErrorBlockStartException : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };

    static const std::string SERVER_BLOCK_DIRECTIVE[3];
    static const int DIRECTIVE_NAME;
    static const int DIRECTIVE_VALUE;
    static const int SERVER_BRACES_START;
    static const int LOCATION_BRACES_START;

    void readAndSplitFile(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);
    void parseFile();
    void parseMainContext();
    void parseServerContext();
    void parseLocationContext(/*, ServerConfig &server*/);
    bool isServerContext();
    bool isLocationContext();
    void putSplitLines();// 後で消す

    std::vector<std::vector<std::string> > config_file_;
    int num_line_;
    Config &config_;
    ConfState state_;
};

#endif /* CONFIGPARSER_HPP */
