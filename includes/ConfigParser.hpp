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
        CGI_EXTENSIONS,
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

    enum DirectiveArgs
    {
        ONE_ARG,
        MULTIPLE_ARGS
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
    void parseCgiExtensions(T &config_obj);
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
    void setDefaultToUnsetMainValue(MainConfig &main_config);
    void setDefaultToUnsetServerValue(ServerConfig &server_config, const MainConfig &main_config);
    void setDefaultToUnsetLocationValue(LocationConfig &location_config, const ServerConfig &server_config);
    template <typename T>
    void setAllowMethodParams(T &config_obj, const std::vector<std::string> &params);
    template <typename T>
    void setCgiExtensionParams(T &config_obj, const std::vector<std::string> &params);
    template <typename T>
    void setErrorPageParams(T &config_obj, const std::map<int, std::string> &params);
    template <typename T>
    void setIndexParams(T &config_obj, const std::vector<std::string> &params);
    static std::vector<ContextType> generateAllowedContext(DirectiveType state);
    template <typename T>
    void setReturnRedirectParam(T &config_obj, const std::map<int, std::string> &param);

    long convertNumber(const std::string &str);

    template <typename T>
    bool containsValue(const T &config_obj, std::string &value);
    bool isAllowedDirective();
    bool isDuplicateLocation(const ServerConfig &server_config, const std::string &path);

    void validateStartServerContext();
    void validateStartLocationContext();
    void validateDuplicateValueTypeStr(const std::string &value);
    void validateDuplicateValueTypeInt(const int value);
    void validateNumOfArgs(const int correct_num);
    void validateEndContext();
    void validateEndSemicolon();
    const std::string validateAutoindexValue();
    const std::map<int, std::string> validateErrorPageParams();
    // const std::vector<std::string> validateIndexParams();
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
    validateNumOfArgs(MULTIPLE_ARGS);
    validateEndSemicolon();

    std::vector<std::string> params;
    std::vector<std::string>::iterator value = parse_line_.begin();

    ++value;
    for (; (*value != ";") && (value != parse_line_.end()); ++value)
    {
        if (*value != HTTPRequest::HTTP_GET
            && *value != HTTPRequest::HTTP_POST
            && *value != HTTPRequest::HTTP_DELETE)
        {
            throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
        if (containsValue(config_obj ,*value))
        {
            throw ConfigError(DUPLICATE_VALUE, parse_line_[DIRECTIVE_NAME_INDEX] + "/" + *value,
                              filepath_, (line_pos_ + 1));
        }
        params.push_back(*value);
    }
    setAllowMethodParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseAutoindex(T &config_obj)
{
    validateDuplicateValueTypeStr(config_obj.autoindex());
    validateNumOfArgs(ONE_ARG);
    validateEndSemicolon();
    std::string value = validateAutoindexValue();
    config_obj.setAutoindex(value);
}

template <typename T>
void ConfigParser::parseCgiExtensions(T &config_obj)
{
    validateNumOfArgs(MULTIPLE_ARGS);
    validateEndSemicolon();

    // std::vector<std::string> params;
    std::vector<std::string>::iterator iter = parse_line_.begin();

    ++iter;
    for (; (*iter != ";") && (iter != parse_line_.end()); ++iter)
    {
        // TODO: 既に登録されているかどうかのチェック→各コンテキストに追加する？
        // if (!isExistValue(DIRECTIVE_TYPE, value))
        // {
        //    throw エラー
        // }
        config_obj.addCgiExtensions(*iter);
    }
}

template <typename T>
void ConfigParser::parseClientMaxBodySize(T &config_obj)
{
    validateDuplicateValueTypeInt(config_obj.clientMaxBodySize());
    validateNumOfArgs(ONE_ARG);
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
    std::map<int, std::string> params = validateErrorPageParams();
    setErrorPageParams(config_obj, params);
}

template <typename T>
void ConfigParser::parseIndex(T &config_obj)
{
    validateNumOfArgs(MULTIPLE_ARGS);
    validateEndSemicolon();
    //TODO: 引数の取得, 引数を追加（重複チェック(findして見つかったらスキップ）
    // std::vector<std::string> params = validateIndexParams();

    // std::vector<std::string> params;
    std::vector<std::string>::iterator iter = parse_line_.begin();

    ++iter;
    for (; (*iter != ";") && (iter != parse_line_.end()); ++iter)
    {
        // TODO: 既に登録されているかどうかのチェック→各コンテキストに追加する？
        // if (!isExistValue(DIRECTIVE_TYPE, value))
        // {
        //    throw エラー
        // }
        config_obj.addIndex(*iter);
    }
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
    validateDuplicateValueTypeStr(config_obj.uploadPath());
    validateNumOfArgs(ONE_ARG);
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
void ConfigParser::setCgiExtensionParams(T &config_obj, const std::vector<std::string> &params)
{
    for (std::vector<std::string>::const_iterator const_iter = params.begin();
         const_iter != params.end();
         ++const_iter)
    {
        config_obj.addCgiExtensions(*const_iter);
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

template <typename T>
bool ConfigParser::containsValue(const T &config_obj, std::string &value)
{
    std::vector<std::string>::iterator found;
    std::vector<std::string> directive;

    switch (directive_type_)
    {
    case ALLOW_METHOD:
        directive = config_obj.allowMethod();
        break;
    case CGI_EXTENSIONS:
        directive = config_obj.cgiExtensions();
        break;
    case INDEX:
        directive = config_obj.index();
        break;
    default:
        break;
    }
    found = std::find(directive.begin(), directive.end(), value);
    if (found != directive.end())
    {
        return true;
    }
    return false;
}

#endif /* CONFIGPARSER_HPP */
