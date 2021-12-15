#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
#include "ConfigErrorType.hpp"
#include "ConfigError.hpp"
#include "LocationConfig.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

const int ConfigParser::NUM_MAIN_DIRECTIVE = 7;
const int ConfigParser::NUM_SERVER_DIRECTIVE = 10;
const int ConfigParser::NUM_LOCATION_DIRECTIVE = 7;

const std::string ConfigParser::MAIN_DIRECTIVE[NUM_MAIN_DIRECTIVE] = {
    "allow_method",
    "autoindex",
    "cgi_extension",
    "client_max_body_size",
    "error_page",
    "index",
    "server"
};

const std::string ConfigParser::SERVER_DIRECTIVE[NUM_SERVER_DIRECTIVE] = {
    "allow_method",
    "autoindex",
    "client_max_body_size",
    "error_page",
    "index",
    "listen",
    "location",
    "return",
    "server_name",
    "upload_path"
};

const std::string ConfigParser::LOCATION_DIRECTIVE[NUM_LOCATION_DIRECTIVE] = {
    "alias",
    "allow_method",
    "autoindex",
    "error_page",
    "index",
    "return",
    "upload_path"
};
const int ConfigParser::DIRECTIVE_NAME = 0;
const int ConfigParser::DIRECTIVE_VALUE = 1;
const int ConfigParser::SERVER_OPEN_BRACES = 1;

ConfigParser::ConfigParser(Config &config)
: num_line_(1),
  config_(config)
{
    setDirectiveInfo();
    setParseFunction();
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::setDirectiveInfo()
{
    directive_info_.insert(std::make_pair(ALIAS, getAllowedContext(ALIAS)));
    directive_info_.insert(std::make_pair(ALLOW_METHOD, getAllowedContext(ALLOW_METHOD)));
    directive_info_.insert(std::make_pair(AUTOINDEX, getAllowedContext(AUTOINDEX)));
    directive_info_.insert(std::make_pair(CGI_EXTENSION, getAllowedContext(CGI_EXTENSION)));
    directive_info_.insert(std::make_pair(CLIENT_MAX_BODY_SIZE, getAllowedContext(CLIENT_MAX_BODY_SIZE)));
    directive_info_.insert(std::make_pair(ERROR_PAGE, getAllowedContext(ERROR_PAGE)));
    directive_info_.insert(std::make_pair(INDEX, getAllowedContext(INDEX)));
    directive_info_.insert(std::make_pair(LISTEN, getAllowedContext(LISTEN)));
    directive_info_.insert(std::make_pair(LOCATION, getAllowedContext(LOCATION)));
    directive_info_.insert(std::make_pair(RETURN, getAllowedContext(RETURN)));
    directive_info_.insert(std::make_pair(SERVER, getAllowedContext(SERVER)));
    directive_info_.insert(std::make_pair(SERVER_NAME, getAllowedContext(SERVER_NAME)));
    directive_info_.insert(std::make_pair(UPLOAD_PATH, getAllowedContext(UPLOAD_PATH)));
}

void ConfigParser::setParseFunction()
{
    directive_parse_func_.insert(std::make_pair(ALIAS, &ConfigParser::parseAlias));
    directive_parse_func_.insert(std::make_pair(ALLOW_METHOD, &ConfigParser::parseAllowMethod));
    directive_parse_func_.insert(std::make_pair(AUTOINDEX, &ConfigParser::parseAutoindex));
    directive_parse_func_.insert(std::make_pair(CGI_EXTENSION, &ConfigParser::parseCgiExtension));
    directive_parse_func_.insert(std::make_pair(CLIENT_MAX_BODY_SIZE, &ConfigParser::parseClientMaxBodySize));
    directive_parse_func_.insert(std::make_pair(ERROR_PAGE, &ConfigParser::parseErrorPage));
    directive_parse_func_.insert(std::make_pair(INDEX, &ConfigParser::parseIndex));
    directive_parse_func_.insert(std::make_pair(LISTEN, &ConfigParser::parseListen));
    directive_parse_func_.insert(std::make_pair(LOCATION, &ConfigParser::parseLocation));
    directive_parse_func_.insert(std::make_pair(RETURN, &ConfigParser::parseReturn));
    directive_parse_func_.insert(std::make_pair(SERVER, &ConfigParser::parseServer));
    directive_parse_func_.insert(std::make_pair(SERVER_NAME, &ConfigParser::parseServerName));
    directive_parse_func_.insert(std::make_pair(UPLOAD_PATH, &ConfigParser::parseUploadPath));
}

void ConfigParser::setDirectiveType(const std::string &directive_name)
{
    if (directive_name == "alias")
        directive_type_ = ALIAS;
    else if (directive_name == "allow_method")
        directive_type_ = ALLOW_METHOD;
    else if (directive_name == "autoindex")
        directive_type_ = AUTOINDEX;
    else if (directive_name == "cgi_extension")
        directive_type_ = CGI_EXTENSION;
    else if (directive_name == "client_max_body_size")
        directive_type_ = CLIENT_MAX_BODY_SIZE;
    else if (directive_name == "error_page")
        directive_type_ = ERROR_PAGE;
    else if (directive_name == "index")
        directive_type_ = INDEX;
    else if (directive_name == "listen")
        directive_type_ = LISTEN;
    else if (directive_name == "location")
        directive_type_ = LOCATION;
    else if (directive_name == "return")
        directive_type_ = RETURN;
    else if (directive_name == "server")
        directive_type_ = SERVER;
    else if (directive_name == "server_name")
        directive_type_ = SERVER_NAME;
    else if (directive_name == "upload_path")
        directive_type_ = UPLOAD_PATH;
    else
        throw ConfigError(UNKNOWN_DIRECTIVE, directive_name, filepath_, num_line_);
}

void ConfigParser::setContextType(ContextType type)
{
    context_type_ = type;
}

void ConfigParser::parseDirective(std::vector<std::string> &parse_line)
{
    std::map<DirectiveType, parse_func>::iterator miter;

    miter = directive_parse_func_.find(directive_type_);
    (this->*miter->second)(parse_line);
}

void ConfigParser::parseAlias(std::vector<std::string> &parse_line)
{
    std::cout << "parseAlias(): size[" << parse_line.size() << "]" << std::endl;
    // 要素数チェック
    // 構文チェック
        // ; で終わっているか
        // directive_valueのチェック（適切な型か、値か...)
}

void ConfigParser::parseAllowMethod(std::vector<std::string> &parse_line)
{
    std::cout << "parseAllowMethod(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseAutoindex(std::vector<std::string> &parse_line)
{
    std::cout << "parseAutoindex(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseCgiExtension(std::vector<std::string> &parse_line)
{
    std::cout << "parseCgiExtension(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseClientMaxBodySize(std::vector<std::string> &parse_line)
{
    std::cout << "parseClientMaxBodySize(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseErrorPage(std::vector<std::string> &parse_line)
{
    std::cout << "parseErrorPage(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseIndex(std::vector<std::string> &parse_line)
{
    std::cout << "parseIndex(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseListen(std::vector<std::string> &parse_line)
{
    std::cout << "parseListen(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseLocation(std::vector<std::string> &parse_line)
{
    std::cout << "parseLocation(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseReturn(std::vector<std::string> &parse_line)
{
    std::cout << "parseReturn(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseServer(std::vector<std::string> &parse_line)
{
    std::cout << "parseServer(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseServerName(std::vector<std::string> &parse_line)
{
    std::cout << "parseServerName(): size[" << parse_line.size() << "]" << std::endl;
}
void ConfigParser::parseUploadPath(std::vector<std::string> &parse_line)
{
    std::cout << "parseUploadPath(): size[" << parse_line.size() << "]" << std::endl;
}

const std::vector<ConfigParser::ContextType> ConfigParser::getAllowedContext(DirectiveType state)
{
    std::vector<ContextType> allowed_context;

    switch (state)
    {
    case ALLOW_METHOD:
    case AUTOINDEX:
    case ERROR_PAGE:
    case INDEX:
    case RETURN:
        allowed_context.push_back(CONTEXT_MAIN);
        allowed_context.push_back(CONTEXT_SERVER);
        allowed_context.push_back(CONTEXT_LOCATION);
        break;
    case LISTEN:
    case LOCATION:
    case SERVER_NAME:
        allowed_context.push_back(CONTEXT_SERVER);
        break;
    case ALIAS:
        allowed_context.push_back(CONTEXT_LOCATION);
        break;
    case CGI_EXTENSION:
    case SERVER:
        allowed_context.push_back(CONTEXT_MAIN);
        break;
    case CLIENT_MAX_BODY_SIZE:
        allowed_context.push_back(CONTEXT_MAIN);
        allowed_context.push_back(CONTEXT_SERVER);
        break;
    case UPLOAD_PATH:
        allowed_context.push_back(CONTEXT_SERVER);
        allowed_context.push_back(CONTEXT_LOCATION);
        break;
    default:
        break;
    }
    return allowed_context;
}

void ConfigParser::readFile(const std::string &filepath)
{
    filepath_ = filepath;
    std::ifstream ifs(filepath);

    if (!ifs)
    {
        throw SystemError("open", errno);
    }
    readAndSplitLines(ifs);
    putSplitLines();// 後で消す
    ifs.close();
    parseMainContext();
}

void ConfigParser::readAndSplitLines(std::ifstream &ifs)
{
    std::string line;

    while (std::getline(ifs, line))
    {
        const std::vector<std::string> words = splitLine(line);
        config_file_.push_back(words);
    }
}

std::vector<std::string> ConfigParser::splitLine(const std::string &line)
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while (line[start])
    {
        while (isspace(line[start]))
        {
            ++start;
        }
        end = start;
        while (isprint(line[end]) && !(isspace(line[end])) && (line[end] != ';'))
        {
            ++end;
        }
        if ((end - start) != 0)
            words.push_back(line.substr(start, (end - start)));
        start = end;
        if (line[start] == ';')
        {
            words.push_back(";");
            start++;
        }
    }
    return (words);
}

// 必要なくなったら消す
void ConfigParser::putSplitLines()
{
    size_t line_num = 1;
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = config_file_.begin();
         vviter != config_file_.end();
         ++vviter)
    {
        std::cout << "------------------(" << line_num << std::endl;
        for (std::vector<std::string>::const_iterator viter = vviter->begin();
             viter != vviter->end();
             ++viter)
        {
            std::cout << "[" << *viter << "]" << std::endl;
        }
        ++line_num;
    }
    std::cout << "-----[end]-----------------------------------\n" << std::endl;
}
// 必要なくなったら消す

void ConfigParser::parseMainContext()
{
    MainConfig main_config = MainConfig();
    std::vector<std::string> parse_line;

    setContextType(CONTEXT_MAIN);
    for (size_t index_line = 0; index_line < config_file_.size(); ++index_line, ++num_line_)
    {
        parse_line.clear();
        parse_line = config_file_[index_line];
        if (parse_line.size() == 0)
        {
            continue;
        }
        setDirectiveType(parse_line[DIRECTIVE_NAME]);
        if (!isAllowDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIECTIVE, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
        }
        parseDirective(parse_line);
        // parseDirective()でswith使って処理の振り分け
        // parseDirectiveの中でセットしようとしているが、処理を分けてもいいかも
            // parseDirective()
            // setDirectiveValue()
    }
}

// void ConfigParser::parseServerContext(MainConfig &main_config)
// {
//     ServerConfig server_config = ServerConfig(main_config);
//     void (ConfigParser::*server_directive_func[NUM_SERVER_DIRECTIVE])(ServerConfig&) = {
//         &ConfigParser::parseAllowMethodDirective,
//         &ConfigParser::parseAutoindexDirective,
//         &ConfigParser::parseClientMaxBodySizeDirective,
//         &ConfigParser::parseErrorPageDirective,
//         &ConfigParser::parseIndexDirective,
//         &ConfigParser::parseListenDirective,
//         &ConfigParser::parseLocationContext,
//         &ConfigParser::parseReturnDirective,
//         &ConfigParser::parseServerNameDirective,
//         &ConfigParser::parseUploadPath
//     };
//     ++viter_config_file;
//     ++num_line_;
//     for (; viter_config_file != config_file_.end(); ++viter_config_file, ++num_line_)
//     {
//         if (viter_config_file->size() == 0)
//         {
//             continue;
//         }
//         parse_line = viter_config_file->begin();
//         if  (parse_line[DIRECTIVE_NAME] == "}")
//         {
//             break;
//         }
//         for (int i = 0; i < NUM_SERVER_DIRECTIVE; ++i)
//         {
//             if (parse_line[DIRECTIVE_NAME] == SERVER_DIRECTIVE[i])
//             {
//                 std::cout << "Line." << num_line_ << " " << SERVER_DIRECTIVE[i] << std::endl; // 後で消す
//                 (this->*server_directive_func[i])(server_config);
//             }
//         }
//     }

//     // 許可しないディレクティブのエラー
//     config_.addServer(server_config);
// }

// void ConfigParser::parseDirective()
// {
//     switch (directive_type_)
//     {
//     case ALIAS:
//         /* code */
//         break;
//     case ALLOW_METHOD:
//         break;
//     case AUTOINDEX:
//         break;
//     case CGI_EXTENSION:
//         break;
//     case CLIENT_MAX_BODY_SIZE:
//         break;
//     case ERROR_PAGE:
//         break;
//     case INDEX:
//         break;
//     case LISTEN:
//         break;
//     case LOCATION:
//         break;
//     case RETURN:
//         break;
//     case SERVER_NAME:
//         break;
//     case UPLOAD_PATH:
//         break;
//     default:
//         break;
//     }
// }

// void ConfigParser::parseLocationContext(ServerConfig &server_config)
// {
//     LocationConfig location_config = LocationConfig(server_config);
//     void (ConfigParser::*location_directive_func[NUM_LOCATION_DIRECTIVE])(LocationConfig&) = {
//         &ConfigParser::parseAliasDirective,
//         &ConfigParser::parseAllowMethodDirective,
//         &ConfigParser::parseAutoindexDirective,
//         &ConfigParser::parseErrorPageDirective,
//         &ConfigParser::parseIndexDirective,
//         &ConfigParser::parseReturnDirective,
//         &ConfigParser::parseUploadPath
//     };
//     ++viter_config_file;
//     ++num_line_;
//     for (; viter_config_file != config_file_.end(); ++viter_config_file, ++num_line_)
//     {
//         if (viter_config_file->size() == 0)
//         {
//             continue;
//         }
//         parse_line = viter_config_file->begin();

//         std::cout << "parse_line_word: " << parse_line->size() << std::endl;//del
//         if  (parse_line[DIRECTIVE_NAME] == "}")
//         {
//             break;
//         }
//         for (int i = 0; i < NUM_LOCATION_DIRECTIVE; ++i)
//         {
//             if (parse_line[DIRECTIVE_NAME] == LOCATION_DIRECTIVE[i])
//             {
//                 std::cout << "Line." << num_line_ << " " << LOCATION_DIRECTIVE[i] << std::endl; // 後で消す
//                 (this->*location_directive_func[i])(location_config);
//             }
//         }
//     }

//     // 許可しないディレクティブのエラー

//     server_config.addLocation(parse_line[1], location_config);
// }

// void ConfigParser::parseCgiExtensionDirective(MainConfig &main_config)
// {
//     //構文チェック
//     main_config.setcgiExtension(parse_line[DIRECTIVE_VALUE]);
// }

// void ConfigParser::parseListenDirective(ServerConfig &server_config)
// {
//     // 構文チェック

//     server_config.setListen(std::atoi(parse_line[DIRECTIVE_VALUE].c_str()));
// }

// void ConfigParser::parseServerNameDirective(ServerConfig &server_config)
// {
//     // 構文チェック

//     server_config.setServerName(parse_line[DIRECTIVE_VALUE]);
// }

bool ConfigParser::isAllowDirective()
{
    std::map<DirectiveType, std::vector<ContextType> >::iterator miter;
    miter = directive_info_.find(directive_type_);

    for (std::vector<ContextType>::iterator viter = miter->second.begin();
         viter != miter->second.end();
         viter++)
    {
        if (*viter == context_type_)
            return true;
    }
    return false;
}


bool ConfigParser::isServerContext(std::vector<std::vector<std::string> > ::const_iterator vviter)
{
    std::vector<std::string>::const_iterator viter = vviter->begin();

    if ((viter[DIRECTIVE_NAME] == "server")
        && (viter[SERVER_OPEN_BRACES] == "{")
        && (vviter->size() == 2))
    {
        return true;
    }
    return false;
}

// bool ConfigParser::isLocationContext(std::vector<std::vector<std::string> > ::const_iterator vviter)
// {
//     std::vector<std::string>::const_iterator viter = vviter->begin();

//     if ((config_file_.at(line_num).at(0) == "location")
//               && (config_file_.at(line_num).at(2) == "{")
//               && (config_file_.at(line_num).size() == 3))
//     if (*viter == "location"
//         && *(++viter) == "{")
//     {
//         return true;
//     }
//     return false;
// }
