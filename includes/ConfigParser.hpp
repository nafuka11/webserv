#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
public:
    enum ContextState
    {
        MAIN_BLOCK,
        SERVER_BLOCK,
        LOCATION_BLOCK
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
    void putSplitLines(std::vector<std::vector<std::string> > &config_file);//必要なくなったら消す
    Config &config_;
    // ParseState state_;
};

#endif /* CONFIGPARSER_HPP */
