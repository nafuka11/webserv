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

    void readAndSplitFile(std::ifstream &ifs, std::vector<std::vector<std::string> > &config_file);
    std::vector<std::string> splitLine(const std::string &line);
    void parseFile(std::vector<std::vector<std::string> > &config_file);
    void parseMainContext(std::vector<std::vector<std::string> > &loaded_file, size_t &line_num);
    void parseServerContext(std::vector<std::vector<std::string> > &loaded_file, size_t &line_num);
    void parseLocationContext(std::vector<std::vector<std::string> > &loaded_file, size_t &line_num, ServerConfig &server);
    void putSplitLines(std::vector<std::vector<std::string> > &config_file);// 後で消す

    Config &config_;
    ConfState state_;
};

#endif /* CONFIGPARSER_HPP */
