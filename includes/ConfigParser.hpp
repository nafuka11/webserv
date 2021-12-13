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
    static const int DIRECTIVE_VALUE;

    static const int SERVER_OPEN_BRACES;
    static const int LOCATION_OPEN_BRACES;

    void readAndSplitLines(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);
    void parseLines();
    void parseMainContext(MainConfig &main_config);
    void parseMainContext();
    void parseServerContext(MainConfig &main_config);
    void parseLocationContext(ServerConfig &server_config);

    template <typename T>
    void parseAllowMethodDirective(T &config_obj);
    template <typename T>
    void parseAutoindexDirective(T &config_obj);
    void parseCgiExtensionDirective(MainConfig &main_config);
    template <typename T>
    void parseClientMaxBodySizeDirective(T &config_obj);
    template <typename T>
    void parseErrorPageDirective(T &config_obj);
    template <typename T>
    void parseIndexDirective(T &config_obj);
    void parseListenDirective(ServerConfig &server_config);
    template <typename T>
    void parseReturnDirective(T &config_obj);
    void parseServerNameDirective(ServerConfig &server_config);
    template <typename T>
    void parseUploadPath(T &config_obj);

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

template <typename T>
void ConfigParser::parseAllowMethodDirective(T &config_obj)
{
    // 構文チェック
    config_obj.clearAllowMethod();
    ++parse_line_word_;

    for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        config_obj.addAllowMethod(*parse_line_word_);
    }
}

template <typename T>
void ConfigParser::parseAutoindexDirective(T &config_obj)
{
    //構文チェック
    config_obj.setAutoIndex(parse_line_word_[DIRECTIVE_VALUE]);
}

template <typename T>
void ConfigParser::parseClientMaxBodySizeDirective(T &config_obj)
{
    //構文チェック
    config_obj.setClientMaxBodySize(std::atoi(parse_line_word_[DIRECTIVE_VALUE].c_str()));
}

template <typename T>
void ConfigParser::parseErrorPageDirective(T &config_obj)
{
    //構文チェック

    std::vector<std::string>::const_iterator status_code;
    std::vector<std::string>::const_iterator uri;

    status_code = parse_line_->begin();
    ++status_code;
    uri = parse_line_->end();
    uri = uri - 2;
    // ex) error_page 404 /404.html;
    if (parse_line_->size() == 4)
    {
        config_obj.clearErrorPage(std::atoi(status_code->c_str()));
        config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
        return ;
    }
    // ex) error_page 500 502 503 504 /50x.html;
    for (; status_code != uri; ++status_code)
    {
        config_obj.clearErrorPage(std::atoi(status_code->c_str()));
        config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
    }
}

template <typename T>
void ConfigParser::parseIndexDirective(T &config_obj)
{
    // 構文チェック
    config_obj.clearIndex();
    ++parse_line_word_;

    for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        config_obj.addIndex(*parse_line_word_);
    }
}

template <typename T>
void ConfigParser::parseReturnDirective(T &config_obj)
{
    // 構文チェック
    config_obj.addReturnRedirect(std::atoi(parse_line_word_[1].c_str()), parse_line_word_[2]);
}

template <typename T>
void ConfigParser::parseUploadPath(T &config_obj)
{
    // 構文チェック
    config_obj.setUploadPath(parse_line_word_[DIRECTIVE_VALUE]);
}

#endif /* CONFIGPARSER_HPP */
