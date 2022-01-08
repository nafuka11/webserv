#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include <iostream> // TODO: 後で消す
#include "ConfigError.hpp"
#include "MainConfig.hpp"
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

    static const int DIRECTIVE_NAME_INDEX;
    static const int DIRECTIVE_VALUE_INDEX;
    static const int SERVER_OPEN_BRACE_INDEX;
    static const int LOCATION_OPEN_BRACE_INDEX;

    static std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > createAllowedDirective();
    static std::map<ConfigParser::DirectiveType, main_parse_func> createMainParseFunc();
    static std::map<ConfigParser::DirectiveType, server_parse_func> createServerParseFunc();
    static std::map<ConfigParser::DirectiveType, location_parse_func> createLocationParseFunc();

    void readAndSplitLines(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);

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
    void parseReturnRedirect(T &config_obj);
    template <typename T>
    void parseUploadPath(T &config_obj);
    void setDirectiveType(const std::string &directive_name);
    void setContextType(ContextType type);
    void initServerConfigFromMain(ServerConfig &server_config, const MainConfig &main_config);
    void initLocationConfigFromServer(LocationConfig &location_config, const ServerConfig &server_config);
    template <typename T>
    void setAllowMethodParams(T &config_obj, const std::vector<std::string> &params);
    template <typename T>
    void setErrorPageParams(T &config_obj, const std::map<int, std::string> &params);
    template <typename T>
    void setIndexParams(T &config_obj, const std::vector<std::string> &params);
    static std::vector<ContextType> generateAllowedContext(DirectiveType state);
    template <typename T>
    void setReturnRedirectParam(T &config_obj, const std::map<int, std::string> &param);

    bool isAllowedDirective();
    bool isDuplicateLocation(const ServerConfig &server_config, const std::string &path);
    bool isCorrectNumOfArgs(const int correct_num);
    bool isCorrectAutoindexValue();

    void validateStartServerContext();
    void validateStartLocationContext();
    void validateEndContext();
    void validateEndSemicolon();
    const std::vector<std::string> validateAllowMethodParams();
    const std::map<int, std::string> validateErrorPageParams();
    const std::vector<std::string> validateIndexParams();
    const std::map<int, std::string> validateReturnParam();

    void putSplitLines(); // TODO: 後で消す

    std::string filepath_;
    std::vector<std::vector<std::string> > parse_lines_;
    std::vector<std::string> parse_line_;
    size_t line_pos_;
    Config &config_;
    ContextType context_type_;
    DirectiveType directive_type_;
};

template <typename T>
void ConfigParser::parseAllowMethod(T &config_obj)
{
    std::vector<std::string> params = validateAllowMethodParams();

    config_obj.clearAllowMethod();
    setAllowMethodParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseAutoindex(T &config_obj)
{
    if (!isCorrectNumOfArgs(1))
    {
        throw ConfigError(INVALID_NUM_OF_ARGS, parse_line_[DIRECTIVE_NAME_INDEX], filepath_, (line_pos_ + 1));
    }
    if (!isCorrectAutoindexValue())
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX], filepath_, (line_pos_ + 1));
    }
    validateEndSemicolon();
    config_obj.setAutoindex(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

template <typename T>
void ConfigParser::parseClientMaxBodySize(T &config_obj)
{
    if (!isCorrectNumOfArgs(1))
    {
        throw ConfigError(INVALID_NUM_OF_ARGS, parse_line_[DIRECTIVE_NAME_INDEX], filepath_, (line_pos_ + 1));
    }
    // TODO: 引数の値(数値か、適切な値か)チェック
    if (!isCorrectClientMaxBodySizeValue())
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX], filepath_, (line_pos_ + 1));
    }
    validateEndSemicolon();
    config_obj.setClientMaxBodySize(std::atoi(parse_line_[DIRECTIVE_VALUE_INDEX].c_str()));
}

template <typename T>
void ConfigParser::parseErrorPage(T &config_obj)
{
    std::map<int, std::string> params = validateErrorPageParams();
    setErrorPageParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseIndex(T &config_obj)
{
    std::vector<std::string> params = validateIndexParams();

    config_obj.clearIndex();
    setIndexParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseReturnRedirect(T &config_obj)
{
    std::map<int, std::string> param = validateReturnParam();
    setReturnRedirectParam(config_obj, param);
}

template <typename T>
void ConfigParser::parseUploadPath(T &config_obj)
{
    if (!isCorrectNumOfArgs(1))
    {
        throw ConfigError(INVALID_NUM_OF_ARGS, parse_line_[DIRECTIVE_NAME_INDEX], filepath_, (line_pos_ + 1));
    }
    validateEndSemicolon();
    config_obj.setUploadPath(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

template <typename T>
void ConfigParser::setAllowMethodParams(T &config_obj, const std::vector<std::string> &params)
{
    for (std::vector<std::string>::const_iterator const_iter = params.begin();
         const_iter != params.end();
         ++const_iter)
    {
        config_obj.addAllowMethod(*const_iter);
    }
}

template <typename T>
void ConfigParser::setErrorPageParams(T &config_obj, const std::map<int, std::string> &params)
{
    for (std::map<int, std::string>::const_iterator const_iter = params.begin();
         const_iter != params.end();
         ++const_iter)
    {
        config_obj.clearErrorPage(const_iter->first); // TODO: addErrorPageの中に入れる？
        config_obj.addErrorPage(const_iter->first, const_iter->second);
    }
}

template <typename T>
void ConfigParser::setIndexParams(T &config_obj, const std::vector<std::string> &params)
{
    for (std::vector<std::string>::const_iterator const_iter = params.begin();
         const_iter != params.end();
         ++const_iter)
    {
        config_obj.addIndex(*const_iter);
    }
}

template <typename T>
void ConfigParser::setReturnRedirectParam(T &config_obj, const std::map<int, std::string> &param)
{
    for (std::map<int, std::string>::const_iterator const_iter = param.begin();
         const_iter != param.end();
         ++const_iter)
    {
        config_obj.clearReturnRedirect(const_iter->first);
        config_obj.addReturnRedirect(const_iter->first, const_iter->second);
    }
}

#endif /* CONFIGPARSER_HPP */
