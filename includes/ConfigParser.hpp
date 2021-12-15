#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>
#include "ServerConfig.hpp"

class Config;

class ConfigParser
{
    typedef void (ConfigParser::*parse_func)(std::vector<std::string>&);
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
    static const std::string MAIN_DIRECTIVE[];
    static const std::string SERVER_DIRECTIVE[];
    static const std::string LOCATION_DIRECTIVE[];
    static const int NUM_MAIN_DIRECTIVE;
    static const int NUM_SERVER_DIRECTIVE;
    static const int NUM_LOCATION_DIRECTIVE;

    static const int DIRECTIVE_NAME;
    static const int DIRECTIVE_VALUE;

    static const int SERVER_OPEN_BRACES;
    static const int LOCATION_OPEN_BRACES;

    void setDirectiveInfo();
    void setParseFunction();
    void setDirectiveType(const std::string &directive_name);
    void setContextType(ContextType type);
    const std::vector<ContextType> getAllowedContext(DirectiveType state);
    void readAndSplitLines(std::ifstream &ifs);
    std::vector<std::string> splitLine(const std::string &line);
    void parseLines();
    void parseMainContext();
    void parseServerContext(MainConfig &main_config);
    void parseLocationContext(ServerConfig &server_config);

    void parseDirective(std::vector<std::string> &parse_line);
    void parseAlias(std::vector<std::string> &parse_line);
    void parseAllowMethod(std::vector<std::string> &parse_line);
    void parseAutoindex(std::vector<std::string> &parse_line);
    void parseCgiExtension(std::vector<std::string> &parse_line);
    void parseClientMaxBodySize(std::vector<std::string> &parse_line);
    void parseErrorPage(std::vector<std::string> &parse_line);
    void parseIndex(std::vector<std::string> &parse_line);
    void parseListen(std::vector<std::string> &parse_line);
    void parseLocation(std::vector<std::string> &parse_line);
    void parseReturn(std::vector<std::string> &parse_line);
    void parseServer(std::vector<std::string> &parse_line);
    void parseServerName(std::vector<std::string> &parse_line);
    void parseUploadPath(std::vector<std::string> &parse_line);

    bool isAllowDirective();
    bool isServerContext(std::vector<std::vector<std::string> > ::const_iterator vviter);
    bool isLocationContext(std::vector<std::vector<std::string> > ::const_iterator vviter);

    void putSplitLines();// 後で消す

    std::string filepath_;
    std::vector<std::vector<std::string> > config_file_;
    int num_line_;
    Config &config_;
    std::map<DirectiveType, std::vector<ContextType> > directive_info_;
    std::map<DirectiveType, parse_func> directive_parse_func_;
    ContextType context_type_;
    DirectiveType directive_type_;
};


// // template <typename T>
// // void ConfigParser::parseAliasDirective(T &config_obj)
// // {
// //     //構文チェック
// //     config_obj.setAlias(iter_parse_line_[DIRECTIVE_VALUE]);
// // }

// template <typename T>
// void ConfigParser::parseAllowMethodDirective(T &config_obj)
// {
//     // 構文チェック
//     config_obj.clearAllowMethod();
//     ++iter_parse_line_;

//     for (; iter_parse_line_ != iter_config_file_->end(); ++iter_parse_line_)
//     {
//         if (*iter_parse_line_ == ";")
//             break ;
//         config_obj.addAllowMethod(*iter_parse_line_);
//     }
// }

// template <typename T>
// void ConfigParser::parseAutoindexDirective(T &config_obj)
// {
//     //構文チェック
//     config_obj.setAutoIndex(iter_parse_line_[DIRECTIVE_VALUE]);
// }

// template <typename T>
// void ConfigParser::parseClientMaxBodySizeDirective(T &config_obj)
// {
//     //構文チェック
//     config_obj.setClientMaxBodySize(std::atoi(iter_parse_line_[DIRECTIVE_VALUE].c_str()));
// }

// template <typename T>
// void ConfigParser::parseErrorPageDirective(T &config_obj)
// {
//     //構文チェック

//     std::vector<std::string>::const_iterator status_code;
//     std::vector<std::string>::const_iterator uri;

//     status_code = iter_config_file_->begin();
//     ++status_code;
//     uri = iter_config_file_->end();
//     uri = uri - 2;
//     // ex) error_page 404 /404.html;
//     if (iter_config_file_->size() == 4)
//     {
//         config_obj.clearErrorPage(std::atoi(status_code->c_str()));
//         config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
//         return ;
//     }
//     // ex) error_page 500 502 503 504 /50x.html;
//     for (; status_code != uri; ++status_code)
//     {
//         config_obj.clearErrorPage(std::atoi(status_code->c_str()));
//         config_obj.addErrorPage(std::atoi(status_code->c_str()), *uri);
//     }
// }

// template <typename T>
// void ConfigParser::parseIndexDirective(T &config_obj)
// {
//     // 構文チェック
//     config_obj.clearIndex();
//     ++iter_parse_line_;

//     for (; iter_parse_line_ != iter_config_file_->end(); ++iter_parse_line_)
//     {
//         if (*iter_parse_line_ == ";")
//             break ;
//         config_obj.addIndex(*iter_parse_line_);
//     }
// }

// template <typename T>
// void ConfigParser::parseReturnDirective(T &config_obj)
// {
//     // 構文チェック
//     config_obj.addReturnRedirect(std::atoi(iter_parse_line_[1].c_str()), iter_parse_line_[2]);
// }

// template <typename T>
// void ConfigParser::parseUploadPath(T &config_obj)
// {
//     // 構文チェック
//     config_obj.setUploadPath(iter_parse_line_[DIRECTIVE_VALUE]);
// }

#endif /* CONFIGPARSER_HPP */
