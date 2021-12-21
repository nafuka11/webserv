#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include <iostream>//del
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
public:
    typedef void (ConfigParser::*main_parse_func)(MainConfig&);
    typedef void (ConfigParser::*server_parse_func)(ServerConfig&);
    typedef void (ConfigParser::*location_parse_func)(LocationConfig&);

    enum ContextType
    {
        CONTEXT_MAIN,
        CONTEXT_SERVER,
        CONTEXT_LOCATION
    };

    enum  DirectiveType
    {
        ALIAS,
        ALLOW_METHOD,
        AUTOINDEX,
        CGI_EXTENSION,
        CLIENT_MAX_BODY_SIZE,
        ERROR_PAGE,
        INDEX,
        LISTEN,
        LOCATION,
        RETURN,
        SERVER,
        SERVER_NAME,
        UPLOAD_PATH
    };

    ConfigParser(Config &config);
    ~ConfigParser();
    void readFile(const std::string &filepath);

private:
    static const std::map<DirectiveType, std::vector<ContextType> > ALLOWED_DIRECTIVE;
    static const std::map<DirectiveType, main_parse_func> MAIN_PARSE_FUNC;
    static const std::map<DirectiveType, server_parse_func> SERVER_PARSE_FUNC;
    static const std::map<DirectiveType, location_parse_func> LOCATION_PARSE_FUNC;

    static const int DIRECTIVE_NAME;
    static const int DIRECTIVE_VALUE;

    static const int SERVER_OPEN_BRACES;
    static const int LOCATION_OPEN_BRACES;

    static std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > setAllowedDirective();
    static std::map<ConfigParser::DirectiveType, main_parse_func> setMainParseFunc();
    static std::map<ConfigParser::DirectiveType, server_parse_func> setServerParseFunc();
    static std::map<ConfigParser::DirectiveType, location_parse_func> setLocationParseFunc();
    void setDirectiveType(const std::string &directive_name);
    void setContextType(ContextType type);
    static std::vector<ContextType> getAllowedContext(DirectiveType state);

    void readAndSplitLines(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);
    void parseLines();
    void parseMainContext();
    void parseServerContext(MainConfig &main_config);
    void parseLocationContext(ServerConfig &server_config);
    void parseAlias(LocationConfig &location_config);
    void parseCgiExtension(MainConfig &main_config);
    void parseListen(ServerConfig &server_config);
    void parseServerName(ServerConfig &server_config);
    template <typename T>
    void parseAllowMethod(T &config_obj);
    template <typename T>
    void parseAutoindex(T &config_obj);
    template <typename T>
    void parseClientMaxBodySize(T &config_obj);
    template <typename T>
    void parseErrorPage(T &config_obj);
    template <typename T>
    void parseIndex(T &config_obj);
    template <typename T>
    void parseReturn(T &config_obj);
    template <typename T>
    void parseUploadPath(T &config_obj);

    bool isEndContext();

    const std::vector<std::string> validateParameterAllowMethod();
    const std::map<int, std::string> validateParamaterErrorPage();
    const std::vector<std::string> validateParameterIndex();
    const std::map<int, std::string> validateParamaterReturn();

    void putSplitLines();// 後で消す

    std::string filepath_;
    std::vector<std::vector<std::string> > parse_file_;
    std::vector<std::string> parse_line_;
    size_t line_pos_;
    Config &config_;
    ContextType context_type_;
    DirectiveType directive_type_;
};

template <typename T>
void ConfigParser::parseAllowMethod(T &config_obj)
{
    std::vector<std::string> param = validateParameterAllowMethod();

    config_obj.clearAllowMethod();
    for (std::vector<std::string>::iterator iter = param.begin();
         iter != param.end();
         ++iter)
    {
        config_obj.addAllowMethod(*iter);
    }
}

template <typename T>
void ConfigParser::parseAutoindex(T &config_obj)
{
    config_obj.setAutoindex(parse_line_[DIRECTIVE_VALUE]);
}

template <typename T>
void ConfigParser::parseClientMaxBodySize(T &config_obj)
{
    config_obj.setClientMaxBodySize(std::atoi(parse_line_[DIRECTIVE_VALUE].c_str()));
}

template <typename T>
void ConfigParser::parseErrorPage(T &config_obj)
{
    std::map<int, std::string> param = validateParamaterErrorPage();

    for (std::map<int, std::string>::iterator iter = param.begin();
         iter != param.end();
         ++iter)
    {
        config_obj.clearErrorPage(iter->first);
        config_obj.addErrorPage(iter->first, iter->second);
    }
}

template <typename T>
void ConfigParser::parseIndex(T &config_obj)
{
    std::vector<std::string> param = validateParameterIndex();
    config_obj.clearIndex();

    for (size_t index = 0; index < param.size(); ++index)
    {
        config_obj.addIndex(param[index]);
    }
}

template <typename T>
void ConfigParser::parseReturn(T &config_obj)
{
    std::map<int, std::string> param = validateParamaterReturn();
    std::map<int, std::string>::iterator iter = param.begin();

    config_obj.clearReturnRedirect(iter->first);
    config_obj.addReturnRedirect(iter->first, iter->second);
}

template <typename T>
void ConfigParser::parseUploadPath(T &config_obj)
{
    config_obj.setUploadPath(parse_line_[DIRECTIVE_VALUE]);
}

#endif /* CONFIGPARSER_HPP */
