#include "ConfigParser.hpp"
#include <fstream>
#include <string>
#include "Config.hpp"
#include "ConfigErrorType.hpp"
#include "ConfigError.hpp"
#include "LocationConfig.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

const std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > ConfigParser::ALLOWED_DIRECTIVE = ConfigParser::setAllowedDirective();
const std::map<ConfigParser::DirectiveType, ConfigParser::main_parse_func> ConfigParser::MAIN_PARSE_FUNC = ConfigParser::setMainParseFunc();
const std::map<ConfigParser::DirectiveType, ConfigParser::server_parse_func> ConfigParser::SERVER_PARSE_FUNC = ConfigParser::setServerParseFunc();
const std::map<ConfigParser::DirectiveType, ConfigParser::location_parse_func> ConfigParser::LOCATION_PARSE_FUNC = ConfigParser::setLocationParseFunc();

const int ConfigParser::DIRECTIVE_NAME = 0;
const int ConfigParser::DIRECTIVE_VALUE = 1;
const int ConfigParser::SERVER_OPEN_BRACES = 1;
const int ConfigParser::LOCATION_OPEN_BRACES = 2;

ConfigParser::ConfigParser(Config &config)
: pos_line_(0),
  num_line_(1),
  config_(config)
{
}

ConfigParser::~ConfigParser()
{
}

std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > ConfigParser::setAllowedDirective()
{
    std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > allowed_directive;

    allowed_directive[ALIAS] = getAllowedContext(ALIAS);
    allowed_directive[ALLOW_METHOD] = getAllowedContext(ALLOW_METHOD);
    allowed_directive[AUTOINDEX] = getAllowedContext(AUTOINDEX);
    allowed_directive[CGI_EXTENSION] = getAllowedContext(CGI_EXTENSION);
    allowed_directive[CLIENT_MAX_BODY_SIZE] = getAllowedContext(CLIENT_MAX_BODY_SIZE);
    allowed_directive[ERROR_PAGE] = getAllowedContext(ERROR_PAGE);
    allowed_directive[INDEX] = getAllowedContext(INDEX);
    allowed_directive[LISTEN] = getAllowedContext(LISTEN);
    allowed_directive[LOCATION] = getAllowedContext(LOCATION);
    allowed_directive[RETURN] = getAllowedContext(RETURN);
    allowed_directive[SERVER] = getAllowedContext(SERVER);
    allowed_directive[SERVER_NAME] = getAllowedContext(SERVER_NAME);
    allowed_directive[UPLOAD_PATH] = getAllowedContext(UPLOAD_PATH);
    return allowed_directive;
}

std::vector<ConfigParser::ContextType> ConfigParser::getAllowedContext(DirectiveType state)
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


std::map<ConfigParser::DirectiveType, ConfigParser::main_parse_func> ConfigParser::setMainParseFunc()
{
    std::map<ConfigParser::DirectiveType, main_parse_func> parse_func;

    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] =  &ConfigParser::parseAutoindex;
    parse_func[CGI_EXTENSION] = &ConfigParser::parseCgiExtension;
    parse_func[CLIENT_MAX_BODY_SIZE] = &ConfigParser::parseClientMaxBodySize;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[SERVER] = &ConfigParser::parseServer;
    return parse_func;
}

std::map<ConfigParser::DirectiveType, ConfigParser::server_parse_func> ConfigParser::setServerParseFunc()
{
    std::map<ConfigParser::DirectiveType, server_parse_func> parse_func;

    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] =  &ConfigParser::parseAutoindex;
    parse_func[CLIENT_MAX_BODY_SIZE] = &ConfigParser::parseClientMaxBodySize;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[LISTEN] = &ConfigParser::parseListen;
    parse_func[LOCATION] = &ConfigParser::parseLocationContext;
    parse_func[RETURN] = &ConfigParser::parseReturn;
    parse_func[SERVER_NAME] = &ConfigParser::parseServerName;
    parse_func[UPLOAD_PATH] = &ConfigParser::parseUploadPath;
    return parse_func;
}

std::map<ConfigParser::DirectiveType, ConfigParser::location_parse_func> ConfigParser::setLocationParseFunc()
{
    std::map<ConfigParser::DirectiveType, location_parse_func> parse_func;

    parse_func[ALIAS] = &ConfigParser::parseAlias;
    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] = &ConfigParser::parseAutoindex;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[RETURN] = &ConfigParser::parseReturn;
    parse_func[UPLOAD_PATH] = &ConfigParser::parseUploadPath;
    return parse_func;
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
    for (; pos_line_ < config_file_.size(); ++pos_line_, ++num_line_)
    {
        parse_line.clear();
        parse_line = config_file_[pos_line_];
        if (parse_line.size() == 0)
        {
            continue;
        }
        setDirectiveType(parse_line[DIRECTIVE_NAME]);
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIECTIVE, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
        }
        std::map<DirectiveType, main_parse_func>::const_iterator miter;

        miter = MAIN_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(parse_line, main_config);
    }
}

void ConfigParser::parseServerContext(MainConfig &main_config)
{
    ServerConfig server_config = ServerConfig(main_config);
    std::vector<std::string> parse_line;

    setContextType(CONTEXT_SERVER);
    for (; pos_line_ < config_file_.size(); ++pos_line_, ++num_line_)
    {
        parse_line.clear();
        parse_line = config_file_[pos_line_];

        std::cout << "pos_line_:" << pos_line_ << " " << parse_line[0] << std::endl;
        if (parse_line.size() == 0)
        {
            continue;
        }
        if (isEndContext(parse_line))
        {
            break ;
        }
        setDirectiveType(parse_line[DIRECTIVE_NAME]);
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIECTIVE, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
        }
        std::map<DirectiveType, server_parse_func>::const_iterator miter;

        miter = SERVER_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(parse_line, server_config);
    }
    config_.addServer(server_config);
    setContextType(CONTEXT_MAIN);
}

void ConfigParser::parseLocationContext(std::vector<std::string> &parse_line, ServerConfig &server_config)
{
    std::string location_path = parse_line[DIRECTIVE_VALUE];
    ++pos_line_;
    std::cout << "parseLocationContext(): size[" << parse_line.size() << "]" << std::endl;

    LocationConfig location_config = LocationConfig(server_config);
    setContextType(CONTEXT_LOCATION);

    for (; pos_line_ < config_file_.size(); ++pos_line_, ++num_line_)
    {
        parse_line.clear();
        parse_line = config_file_[pos_line_];

        std::cout << "pos_line_:" << pos_line_ << " " << parse_line[0] << std::endl;
        if (parse_line.size() == 0)
        {
            continue;
        }
        if (isEndContext(parse_line))
        {
            break ;
        }
        setDirectiveType(parse_line[DIRECTIVE_NAME]);
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIECTIVE, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
        }
        std::map<DirectiveType, location_parse_func>::const_iterator miter;

        miter = LOCATION_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(parse_line, location_config);
    }
    server_config.addLocation(location_path, location_config);
    setContextType(CONTEXT_SERVER);
}

void ConfigParser::setContextType(ContextType type)
{
    context_type_ = type;
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

void ConfigParser::parseAlias(std::vector<std::string> &parse_line, LocationConfig &location_config)
{
    std::cout << "parseAlias(): size[" << parse_line.size() << "]" << std::endl;

    // validateNumOfArgs(parse_line, 1);
    validateDirectiveEnd(parse_line);

    location_config.setAlias(parse_line[DIRECTIVE_VALUE]);
}

void ConfigParser::parseCgiExtension(std::vector<std::string> &parse_line, MainConfig &main_config)
{
    std::cout << "parseCgiExtension(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    main_config.setCgiExtension(parse_line[DIRECTIVE_VALUE]);
}

void ConfigParser::parseListen(std::vector<std::string> &parse_line, ServerConfig &server_config)
{
    std::cout << "parseListen(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    server_config.setListen(std::atoi(parse_line[DIRECTIVE_VALUE].c_str()));
}

void ConfigParser::parseLocation(std::vector<std::string> &parse_line, ServerConfig &server_config)
{
    std::cout << "parseLocation(): size[" << parse_line.size() << "]" << std::endl;

    if (!isLocationContext(parse_line))
    {
        throw ConfigError(SYNTAXERROR, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
    }
    parseLocationContext(parse_line, server_config);
}

void ConfigParser::parseServer(std::vector<std::string> &parse_line, MainConfig &main_config)
{
    std::cout << "parseServer(): size[" << parse_line.size() << "]" << std::endl;

    if (!isServerContext(parse_line))
    {
        throw ConfigError(SYNTAXERROR, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
    }
    ++pos_line_;
    parseServerContext(main_config);
}

void ConfigParser::parseServerName(std::vector<std::string> &parse_line, ServerConfig &server_config)
{
    std::cout << "parseServerName(): size[" << parse_line.size() << "]" << std::endl;

    validateDirectiveEnd(parse_line);
    server_config.setServerName(parse_line[DIRECTIVE_VALUE]);
}

bool ConfigParser::isAllowedDirective()
{
    std::map<DirectiveType, std::vector<ContextType> >::const_iterator miter;
    miter = ALLOWED_DIRECTIVE.find(directive_type_);

    for (std::vector<ContextType>::const_iterator viter = miter->second.begin();
         viter != miter->second.end();
         viter++)
    {
        if (*viter == context_type_)
            return true;
    }
    return false;
}


bool ConfigParser::isServerContext(std::vector<std::string> &parse_line)
{
    if ((parse_line[DIRECTIVE_NAME] == "server")
        && (parse_line[SERVER_OPEN_BRACES] == "{")
        && (parse_line.size() == 2))
    {
        return true;
    }
    return false;
}

bool ConfigParser::isLocationContext(std::vector<std::string> &parse_line)
{
    if ((parse_line[DIRECTIVE_NAME] == "server")
        && (parse_line[LOCATION_OPEN_BRACES] == "{")
        && (parse_line.size() == 3))
    {
        return true;
    }
    return false;
}

bool ConfigParser::isEndContext(std::vector<std::string> &parse_line)
{
    if (parse_line.size() == 1 && parse_line[0] == "}")
    {
        return true;
    }
    return false;
}


std::vector<std::string> ConfigParser::validateParameterAllowMethod(std::vector<std::string> &parse_line)
{
    std::vector<std::string> param;
    std::vector<std::string>::iterator iter = parse_line.begin();

    ++iter;
    for (;
         (*iter != ";") && (iter != parse_line.end());
         ++iter)
    {
        if ((*iter != "GET") && (*iter != "POST") && (*iter != "DELETE"))
            throw ConfigError(INVALID_PARAMETER, *iter, filepath_, num_line_);
        param.push_back(*iter);
    }
    return param;
}

std::vector<std::string> ConfigParser::validateParameterIndex(std::vector<std::string> &parse_line)
{
    std::vector<std::string> param;
    std::vector<std::string>::iterator iter = parse_line.begin();

    ++iter;
    for (;
         (*iter != ";") && (iter != parse_line.end());
         ++iter)
    {
        param.push_back(*iter);
    }
    return param;
}


void ConfigParser::validateNumOfArgs(std::vector<std::string> &parse_line, size_t valid_num)
{
    size_t count = 0;

    for (;
         (count < parse_line.size()) && (parse_line[count] != ";");
         ++count)
    {std::cout << count << std::endl;}
    if (count != valid_num)
        throw ConfigError(INVALID_NUM_OF_ARGS, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
}

void ConfigParser::validateDirectiveEnd(std::vector<std::string> &parse_line)
{
    std::vector<std::string>::iterator iter = std::find(parse_line.begin(), parse_line.end(), ";");

    if (iter == parse_line.end())
        throw ConfigError(NO_SEMICOLON, parse_line[DIRECTIVE_NAME], filepath_, num_line_);
    if (++iter != parse_line.end())
        throw ConfigError(UNEXPECTED, *iter, filepath_, num_line_);
}
