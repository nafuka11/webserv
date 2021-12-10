#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
public:
    enum ParseState
    {
        PARSE_MAIN_BLOCK,
        PARSE_SERVER_BLOCK,
        // PARSE_SERVER_BLOCK_START,
        // PARSE_SERVER_BLOCK_DOONE,
        PARSE_LOCATION_BLOCK,
        // PARSE_LOCATION_BLOCK_START,
        // PARSE_LOCATION_BLOCK_DONE
        PARSE_DONE//del
    };

    ConfigParser(Config &config);
    ~ConfigParser();
    void readFile(const std::string &filepath);
    std::vector<std::string> splitLine(const std::string &line);
    void parseWords(const std::vector<std::string> &words);
    void parseMainBlock(const std::vector<std::string> &words);
    void parseServerBlock(const std::vector<std::string> &words);
    // void parseLocationBlock(const std::vector<std::string> &words);
    void parseListenDirective(ServerConfig &server, const std::vector<std::string> &words);
    void parseServerNameDirective(ServerConfig &server, const std::vector<std::string> &words);
private:
    class OpenConfigFileException : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorBlockStartException : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };

    static const std::string SERVER_BLOCK_DIRECTIVE[3];

    Config &config_;
    ParseState state_;
};

#endif /* CONFIGPARSER_HPP */
