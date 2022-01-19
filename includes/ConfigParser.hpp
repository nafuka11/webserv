#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include <iostream> // TODO: 後で消す
#include "ConfigError.hpp"
#include "MainConfig.hpp"
#include "HTTPRequest.hpp"
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

    enum DirectiveNumArgs
    {
        NUM_ONE = 1,
        NUM_TWO,
        NUM_MULTIPLE
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
    static const int PORT_MAX_VALUE;
    static const int PORT_MIN_VALUE;

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
    void parseListen(ServerConfig &server_config);
    void parseServerName(ServerConfig &server_config);
    template <typename T>
    void parseAllowMethod(T &config_obj);
    template <typename T>
    void parseAutoindex(T &config_obj);
    template <typename T>
    void parseCgiExtension(T &config_obj);
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

    void setContextType(ContextType type);
    void setDirectiveType(const std::string &directive_name);
    void setDefaultToUnsetMainValue(MainConfig &main_config);
    void setDefaultToUnsetServerValue(ServerConfig &server_config, const MainConfig &main_config);
    void setDefaultToUnsetLocationValue(LocationConfig &location_config, const ServerConfig &server_config);
    template <typename T>
    void setAllowMethod(T &config_obj, const std::vector<std::string> &values);
    template <typename T>
    void setCgiExtension(T &config_obj, const std::vector<std::string> &values);
    template <typename T>
    void setErrorPageParams(T &config_obj, const std::map<int, std::string> &params);
    template <typename T>
    void setIndex(T &config_obj, const std::vector<std::string> &values);
    static std::vector<ContextType> generateAllowedContext(DirectiveType state);
    template <typename T>
    void setReturnRedirectParam(T &config_obj, const std::map<int, std::string> &param);

    void validateDuplicateValueTypeStr(const std::string &value);
    void validateDuplicateValueTypeInt(const int value);
    void validateContainsValues(std::vector<std::string> &values,
                                const std::vector<std::string> &set_values);
    void validateEndContext();
    void validateEndSemicolon();
    void validateNumOfArgs(DirectiveNumArgs num);
    void validateStartServerContext();
    void validateStartLocationContext();
    const std::map<int, std::string> validateErrorPageParams();
    const std::map<int, std::string> validateReturnParam();

    long convertNumber(const std::string &str);

    bool containsValue(std::string &value, const std::vector<std::string> &set_values);
    bool isAllowedDirective();
    bool isDuplicateLocation(const ServerConfig &server_config, const std::string &path);

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
    validateNumOfArgs(NUM_MULTIPLE);
    validateEndSemicolon();

    std::vector<std::string> values;
    validateContainsValues(values, config_obj.allowMethod());
    for (std::vector<std::string>::const_iterator value = values.begin();
         value != values.end();
         value++)
    {
        if (*value != HTTPRequest::HTTP_GET
            && *value != HTTPRequest::HTTP_POST
            && *value != HTTPRequest::HTTP_DELETE)
        {
            throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
    }
    setAllowMethod(config_obj, values);
}

template <typename T>
void ConfigParser::parseAutoindex(T &config_obj)
{
    validateDuplicateValueTypeStr(config_obj.autoindex());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();

    std::string value = parse_line_[DIRECTIVE_VALUE_INDEX];
    if (value != "on" && value != "off")
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    config_obj.setAutoindex(value);
}

template <typename T>
void ConfigParser::parseCgiExtension(T &config_obj)
{
    validateNumOfArgs(NUM_MULTIPLE);
    validateEndSemicolon();

    std::vector<std::string> values;
    validateContainsValues(values, config_obj.cgiExtension());
    setCgiExtension(config_obj, values);
}

template <typename T>
void ConfigParser::parseClientMaxBodySize(T &config_obj)
{
    validateDuplicateValueTypeInt(config_obj.clientMaxBodySize());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();

    long value = convertNumber(parse_line_[DIRECTIVE_VALUE_INDEX]);
    if (value < 0)
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    config_obj.setClientMaxBodySize(value);
}

template <typename T>
void ConfigParser::parseErrorPage(T &config_obj)
{
    //TODO: 引数の数チェック
    validateNumOfArgs(NUM_TWO);
    //TODO: 第一引数が数値かチェック
    //TODO: 重複チェック
    std::map<int, std::string> params = validateErrorPageParams();
    setErrorPageParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseIndex(T &config_obj)
{
    validateNumOfArgs(NUM_MULTIPLE);
    validateEndSemicolon();

    std::vector<std::string> values;
    validateContainsValues(values, config_obj.index());
    setIndex(config_obj, values);
}

template <typename T>
void ConfigParser::parseReturnRedirect(T &config_obj)
{
    //TODO: 引数の数チェック
    validateNumOfArgs(NUM_TWO);
    //TODO: 第一引数が数値かチェック
    //TODO: 重複チェック
    std::map<int, std::string> param = validateReturnParam();
    setReturnRedirectParam(config_obj, param);
}

template <typename T>
void ConfigParser::parseUploadPath(T &config_obj)
{
    validateDuplicateValueTypeStr(config_obj.uploadPath());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();
    config_obj.setUploadPath(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

template <typename T>
void ConfigParser::setAllowMethod(T &config_obj, const std::vector<std::string> &values)
{
    for (std::vector<std::string>::const_iterator value = values.begin();
         value != values.end();
         ++value)
    {
        config_obj.addAllowMethod(*value);
    }
}

template <typename T>
void ConfigParser::setCgiExtension(T &config_obj, const std::vector<std::string> &values)
{
    for (std::vector<std::string>::const_iterator value = values.begin();
         value != values.end();
         ++value)
    {
        config_obj.addCgiExtension(*value);
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
void ConfigParser::setIndex(T &config_obj, const std::vector<std::string> &values)
{
    for (std::vector<std::string>::const_iterator value = values.begin();
         value != values.end();
         ++value)
    {
        config_obj.addIndex(*value);
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
