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

    static const std::string MAIN_DIRECTIVE[];
    static const std::string SERVER_DIRECTIVE[];
    static const int NUM_MAIN_DIRECTIVE;
    static const int NUM_SERVER_DIRECTIVE;

    static const int DIRECTIVE_NAME;

    static const int SERVER_OPEN_BRACES;
    static const int LOCATION_OPEN_BRACES;

    void readAndSplitLines(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);
    void parseLines();
    void parseMainContext(MainConfig &main_config);
    void parseServerContext(MainConfig &main_config);
    void parseLocationContext(/*, ServerConfig &server*/);

    void parseAllowMethodDirective(MainConfig &main_config);
    void parseAutoindexDirective(MainConfig &main_config);
    void parseCgiExtensionDirective(MainConfig &main_config);
    void parseClientMaxBodySizeDirective(MainConfig &main_config);
    void parseErrorPageDirective(MainConfig &main_config);
    void parseIndexDirective(MainConfig &main_config);

    bool isServerContext(std::vector<std::vector<std::string> > ::const_iterator vviter);
    bool isLocationContext(std::vector<std::vector<std::string> > ::const_iterator vviter);
    void putSplitLines();// 後で消す

    std::vector<std::vector<std::string> > config_file_;
    std::vector<std::vector<std::string> >::const_iterator parse_line_;
    std::vector<std::string>::const_iterator parse_line_word_;
    int num_line_;
    Config &config_;
    ConfState state_;
};

#endif /* CONFIGPARSER_HPP */
