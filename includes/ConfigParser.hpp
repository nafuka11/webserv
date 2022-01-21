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

    static std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> >
        createAllowedDirective();
    static std::map<ConfigParser::DirectiveType, main_parse_func> createMainParseFunc();
    static std::map<ConfigParser::DirectiveType, server_parse_func>
        createServerParseFunc();
    static std::map<ConfigParser::DirectiveType, location_parse_func>
        createLocationParseFunc();
    static std::vector<ContextType> generateAllowedContext(DirectiveType state);

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
    void setDefaultToUnsetServerValue(ServerConfig &server_config,
                                      const MainConfig &main_config);
    void setDefaultToUnsetLocationValue(LocationConfig &location_config,
                                        const ServerConfig &server_config);
    template <typename T>
    void setAllowMethod(T &config_obj, const std::vector<std::string> &values);
    template <typename T>
    void setCgiExtension(T &config_obj, const std::vector<std::string> &values);
    template <typename T>
    void setErrorPage(T &config_obj, const std::map<int, std::string> &pair_values);
    template <typename T>
    void setIndex(T &config_obj, const std::vector<std::string> &values);
    template <typename T>
    void setReturnRedirect(T &config_obj, const std::map<int, std::string> &pair_value);

    void validateEndContext();
    void validateEndSemicolon();
    void validateNumOfArgs(DirectiveNumArgs num);
    void validateStartServerContext();
    void validateStartLocationContext();
    void validateDuplicateValueTypeInt(const int value);
    void validateDuplicateValueTypeStr(const std::string &value);
    void validateContainsValues(std::vector<std::string> &values,
                                const std::vector<std::string> &set_values);
    void validatePairValue(std::map<int, std::string> &pair_value);
    bool containsValue(std::string &value, const std::vector<std::string> &set_values);
    bool isAllowedDirective();
    bool isDuplicateLocation(const ServerConfig &server_config, const std::string &path);

    long convertNumber(const std::string &str);

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
    validateNumOfArgs(NUM_TWO);
    validateEndSemicolon();

    long status_code = convertNumber(parse_line_[DIRECTIVE_VALUE_INDEX]);
    if (status_code < 300 || status_code > 599)
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                             filepath_, (line_pos_ + 1));
    }

    std::map<int, std::string> set_value = config_obj.errorPage();
    std::map<int, std::string>::const_iterator found = set_value.find(status_code);
    if (found != set_value.end())
    {
        throw ConfigError(DUPLICATE_VALUE,
                          parse_line_[DIRECTIVE_NAME_INDEX] + ":" +
                          parse_line_[DIRECTIVE_VALUE_INDEX],
                          filepath_, (line_pos_ + 1));
    }

    std::map<int, std::string> pair_value;
    pair_value.insert(std::make_pair(status_code, parse_line_[DIRECTIVE_VALUE_INDEX + 1]));
    setErrorPage(config_obj, pair_value);
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
    if (!config_obj.returnRedirect().empty())
    {
        throw ConfigError(DUPLICATE_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    validateNumOfArgs(NUM_TWO);
    validateEndSemicolon();

    long status_code = convertNumber(parse_line_[DIRECTIVE_VALUE_INDEX]);
    if (status_code < 0 || status_code > 999)
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                             filepath_, (line_pos_ + 1));
    }

    std::map<int, std::string> pair_value;
    pair_value.insert(std::make_pair(status_code, parse_line_[DIRECTIVE_NAME_INDEX + 1]));
    setReturnRedirect(config_obj, pair_value);
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
void ConfigParser::setErrorPage(T &config_obj,
                                const std::map<int, std::string> &pair_values)
{
    for (std::map<int, std::string>::const_iterator pair_value = pair_values.begin();
         pair_value != pair_values.end();
         ++pair_value)
    {
        config_obj.addErrorPage(pair_value->first, pair_value->second);
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
void ConfigParser::setReturnRedirect(T &config_obj,
                                     const std::map<int, std::string> &pair_value)
{
    std::map<int, std::string>::const_iterator pair_value_iter = pair_value.begin();
    config_obj.addReturnRedirect(pair_value_iter->first, pair_value_iter->second);
}

#endif /* CONFIGPARSER_HPP */
