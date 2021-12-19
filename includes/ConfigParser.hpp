#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include <iostream>//del
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
    typedef void (ConfigParser::*main_parse_func)(std::vector<std::string>&, MainConfig&);
    typedef void (ConfigParser::*server_parse_func)(std::vector<std::string>&, ServerConfig&);
    typedef void (ConfigParser::*location_parse_func)(std::vector<std::string>&, LocationConfig&);
public:
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
    void parseLocationContext(std::vector<std::string> &parse_line, ServerConfig &server_config);
    void parseAlias(std::vector<std::string> &parse_line, LocationConfig &location_config);
    void parseCgiExtension(std::vector<std::string> &parse_line, MainConfig &main_config);
    void parseListen(std::vector<std::string> &parse_line, ServerConfig &server_config);
    void parseLocation(std::vector<std::string> &parse_line, ServerConfig &server_config);
    void parseServer(std::vector<std::string> &parse_line, MainConfig &main_config);
    void parseServerName(std::vector<std::string> &parse_line, ServerConfig &server_config);
    template <typename T>
    void parseAllowMethod(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseAutoindex(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseClientMaxBodySize(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseErrorPage(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseIndex(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseReturn(std::vector<std::string> &parse_line, T &config_obj);
    template <typename T>
    void parseUploadPath(std::vector<std::string> &parse_line, T &config_obj);

    bool isAllowedDirective();
    bool isServerContext(std::vector<std::string> &parse_line);
    bool isLocationContext(std::vector<std::string> &parse_line);
    bool isEndContext(std::vector<std::string> &parse_line);

    std::vector<std::string> validateParameterAllowMethod(std::vector<std::string> &parse_line);
    std::vector<std::string> validateParameterIndex(std::vector<std::string> &parse_line);
    void validateNumOfArgs(std::vector<std::string> &parse_line, size_t valid_num);
    void validateNumOfArgsAllowMethod(std::vector<std::string> &param);
    void validateDirectiveEnd(std::vector<std::string> &parse_line);

    void putSplitLines();// 後で消す

    std::string filepath_;
    std::vector<std::vector<std::string> > config_file_;
    size_t parse_pos_;
    Config &config_;
    ContextType context_type_;
    DirectiveType directive_type_;
};

template <typename T>
void ConfigParser::parseAllowMethod(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseAllowMethod(): size[" << parse_line.size() << "]" << std::endl;

    // paramaterのチェック（適切な型か、値か...)
    std::vector<std::string> param = validateParameterAllowMethod(parse_line);
    // validateNumOfArgsAllowMethod(param);
    validateDirectiveEnd(parse_line);

    config_obj.clearAllowMethod();
    for (size_t index = 0; index < param.size(); ++index)
    {
        config_obj.addAllowMethod(param[index]);
    }
}

template <typename T>
void ConfigParser::parseAutoindex(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseAutoindex(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    config_obj.setAutoindex(parse_line[DIRECTIVE_VALUE]);
}

template <typename T>
void ConfigParser::parseClientMaxBodySize(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseClientMaxBodySize(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    config_obj.setClientMaxBodySize(std::atoi(parse_line[DIRECTIVE_VALUE].c_str()));
}

template <typename T>
void ConfigParser::parseErrorPage(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseErrorPage(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    std::vector<std::string>::const_iterator status_code;
    std::vector<std::string>::const_iterator uri;

    status_code = parse_line.begin();
    ++status_code;

    uri = parse_line.end();
    uri = uri - 2;

    if (parse_line.size() == 4)
    {
        config_obj.clearErrorPage(std::atoi(status_code->c_str()));
        config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
        return;
    }
    for (; status_code  != uri; ++status_code)
    {
        config_obj.clearErrorPage(std::atoi(status_code->c_str()));
        config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
    }
}

template <typename T>
void ConfigParser::parseIndex(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseIndex(): size[" << parse_line.size() << "]" << std::endl;

    // // 構文チェック
    // config_obj.clearIndex();
    // ++parse_line;

    // for (; parse_line != iter_config_file_->end(); ++parse_line)
    // {
    //     if (*parse_line == ";")
    //         break ;
    //     config_obj.addIndex(*parse_line);
    // }
    // paramaterのチェック（適切な型か、値か...)
    std::vector<std::string> param = validateParameterIndex(parse_line);
    // validateNumOfArgsIndex(param);
    validateDirectiveEnd(parse_line);

    config_obj.clearIndex();
    for (size_t index = 0; index < param.size(); ++index)
    {
        config_obj.addIndex(param[index]);
    }
}

template <typename T>
void ConfigParser::parseReturn(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseReturn(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    config_obj.addReturnRedirect(std::atoi(parse_line[1].c_str()), parse_line[2]);
}

template <typename T>
void ConfigParser::parseUploadPath(std::vector<std::string> &parse_line, T &config_obj)
{
    std::cout << "parseReturn(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    config_obj.setUploadPath(parse_line[DIRECTIVE_VALUE]);
}

#endif /* CONFIGPARSER_HPP */
