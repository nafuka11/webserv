#include "ConfigParser.hpp"
#include <fstream>
#include <string>
#include "Config.hpp"
#include "LocationConfig.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

const std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> >
    ConfigParser::ALLOWED_DIRECTIVE = ConfigParser::createAllowedDirective();
const std::map<ConfigParser::DirectiveType, ConfigParser::main_parse_func>
    ConfigParser::MAIN_PARSE_FUNC = ConfigParser::createMainParseFunc();
const std::map<ConfigParser::DirectiveType, ConfigParser::server_parse_func>
    ConfigParser::SERVER_PARSE_FUNC = ConfigParser::createServerParseFunc();
const std::map<ConfigParser::DirectiveType, ConfigParser::location_parse_func>
    ConfigParser::LOCATION_PARSE_FUNC = ConfigParser::createLocationParseFunc();

const int ConfigParser::DIRECTIVE_NAME_INDEX = 0;
const int ConfigParser::DIRECTIVE_VALUE_INDEX = 1;

ConfigParser::ConfigParser(Config &config) : line_pos_(0), config_(config)
{
}

ConfigParser::~ConfigParser()
{
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
    putSplitLines(); // TODO: 後で消す
    ifs.close();
    parseMainContext();
}

void ConfigParser::readAndSplitLines(std::ifstream &ifs)
{
    std::string line;

    while (std::getline(ifs, line))
    {
        const std::vector<std::string> words = splitLine(line);
        parse_lines_.push_back(words);
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
        while (isprint(line[end]) && !(isspace(line[end]))
               && (line[end] != ';') && (line[end] != '{') &&  (line[end] != '}'))
        {
            ++end;
        }
        if ((end - start) != 0)
            words.push_back(line.substr(start, (end - start)));
        if ((line[end] == ';') || (line[end] == '{') || (line[end] == '}'))
        {
            start = end;
            ++end;
            words.push_back(line.substr(start, (end - start)));
        }
        start = end;
    }
    return words;
}

// TODO: 後で消す
void ConfigParser::putSplitLines()
{
    size_t line_num = 1;
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = parse_lines_.begin();
         vviter != parse_lines_.end();
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

void ConfigParser::parseMainContext()
{
    MainConfig main_config = MainConfig();

    setContextType(CONTEXT_MAIN);
    for (; line_pos_ < parse_lines_.size(); ++line_pos_)
    {
        parse_line_.clear();
        parse_line_ = parse_lines_[line_pos_];
        if (parse_line_.size() == 0)
        {
            continue;
        }
        setDirectiveType(parse_line_[DIRECTIVE_NAME_INDEX]);
        std::map<DirectiveType, main_parse_func>::const_iterator miter;
        miter = MAIN_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(main_config);
    }
}

void ConfigParser::parseServerContext(MainConfig &main_config)
{
    ServerConfig server_config = ServerConfig();

    initServerConfigFromMain(server_config, main_config);
    setContextType(CONTEXT_SERVER);
    ++line_pos_;

    for (; line_pos_ < parse_lines_.size(); ++line_pos_)
    {
        parse_line_.clear();
        parse_line_ = parse_lines_[line_pos_];
        if (parse_line_.size() == 0)
        {
            continue;
        }
        if (isEndContext())
        {
            break ;
        }
        setDirectiveType(parse_line_[DIRECTIVE_NAME_INDEX]);

        std::map<DirectiveType, server_parse_func>::const_iterator miter;
        miter = SERVER_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(server_config);
    }
    config_.addServer(server_config);
    setContextType(CONTEXT_MAIN);
}

void ConfigParser::parseLocationContext(ServerConfig &server_config)
{
    LocationConfig location_config = LocationConfig();
    std::string location_path = parse_line_[DIRECTIVE_VALUE_INDEX];

    initLocationConfigFromServer(location_config, server_config);
    setContextType(CONTEXT_LOCATION);
    ++line_pos_;

    for (; line_pos_ < parse_lines_.size(); ++line_pos_)
    {
        parse_line_.clear();
        parse_line_ = parse_lines_[line_pos_];
        if (parse_line_.size() == 0)
        {
            continue;
        }
        if (isEndContext())
        {
            break ;
        }
        setDirectiveType(parse_line_[DIRECTIVE_NAME_INDEX]);

        std::map<DirectiveType, location_parse_func>::const_iterator miter;
        miter = LOCATION_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(location_config);
    }
    server_config.clearLocation(location_path);
    server_config.addLocation(location_path, location_config);
    setContextType(CONTEXT_SERVER);
}

void ConfigParser::parseAlias(LocationConfig &location_config)
{
    location_config.setAlias(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

void ConfigParser::parseCgiExtension(MainConfig &main_config)
{
    main_config.setCgiExtension(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

void ConfigParser::parseListen(ServerConfig &server_config)
{
    server_config.setListen(std::atoi(parse_line_[DIRECTIVE_VALUE_INDEX].c_str()));
}

void ConfigParser::parseServerName(ServerConfig &server_config)
{
    server_config.setServerName(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

bool ConfigParser::isEndContext()
{
    if (parse_line_.size() == 1 && parse_line_[0] == "}")
    {
        return true;
    }
    return false;
}

std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > ConfigParser::createAllowedDirective()
{
    std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> > allowed_directive;

    allowed_directive[ALIAS] = generateAllowedContext(ALIAS);
    allowed_directive[ALLOW_METHOD] = generateAllowedContext(ALLOW_METHOD);
    allowed_directive[AUTOINDEX] = generateAllowedContext(AUTOINDEX);
    allowed_directive[CGI_EXTENSION] = generateAllowedContext(CGI_EXTENSION);
    allowed_directive[CLIENT_MAX_BODY_SIZE] = generateAllowedContext(CLIENT_MAX_BODY_SIZE);
    allowed_directive[ERROR_PAGE] = generateAllowedContext(ERROR_PAGE);
    allowed_directive[INDEX] = generateAllowedContext(INDEX);
    allowed_directive[LISTEN] = generateAllowedContext(LISTEN);
    allowed_directive[LOCATION] = generateAllowedContext(LOCATION);
    allowed_directive[RETURN] = generateAllowedContext(RETURN);
    allowed_directive[SERVER] = generateAllowedContext(SERVER);
    allowed_directive[SERVER_NAME] = generateAllowedContext(SERVER_NAME);
    allowed_directive[UPLOAD_PATH] = generateAllowedContext(UPLOAD_PATH);
    return allowed_directive;
}

std::vector<ConfigParser::ContextType> ConfigParser::generateAllowedContext(DirectiveType state)
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


std::map<ConfigParser::DirectiveType, ConfigParser::main_parse_func> ConfigParser::createMainParseFunc()
{
    std::map<ConfigParser::DirectiveType, main_parse_func> parse_func;

    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] =  &ConfigParser::parseAutoindex;
    parse_func[CGI_EXTENSION] = &ConfigParser::parseCgiExtension;
    parse_func[CLIENT_MAX_BODY_SIZE] = &ConfigParser::parseClientMaxBodySize;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[SERVER] = &ConfigParser::parseServerContext;
    return parse_func;
}

std::map<ConfigParser::DirectiveType, ConfigParser::server_parse_func> ConfigParser::createServerParseFunc()
{
    std::map<ConfigParser::DirectiveType, server_parse_func> parse_func;

    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] =  &ConfigParser::parseAutoindex;
    parse_func[CLIENT_MAX_BODY_SIZE] = &ConfigParser::parseClientMaxBodySize;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[LISTEN] = &ConfigParser::parseListen;
    parse_func[LOCATION] = &ConfigParser::parseLocationContext;
    parse_func[RETURN] = &ConfigParser::parseReturnRedirect;
    parse_func[SERVER_NAME] = &ConfigParser::parseServerName;
    parse_func[UPLOAD_PATH] = &ConfigParser::parseUploadPath;
    return parse_func;
}

std::map<ConfigParser::DirectiveType, ConfigParser::location_parse_func> ConfigParser::createLocationParseFunc()
{
    std::map<ConfigParser::DirectiveType, location_parse_func> parse_func;

    parse_func[ALIAS] = &ConfigParser::parseAlias;
    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] = &ConfigParser::parseAutoindex;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[RETURN] = &ConfigParser::parseReturnRedirect;
    parse_func[UPLOAD_PATH] = &ConfigParser::parseUploadPath;
    return parse_func;
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
}

void ConfigParser::setContextType(ContextType type)
{
    context_type_ = type;
}

void ConfigParser::initServerConfigFromMain(ServerConfig &server_config, const MainConfig &main_config)
{
    setAllowMethodParams(server_config, main_config.allowMethod());
    server_config.setAutoindex(main_config.autoindex());
    server_config.setCgiExtension(main_config.cgiExtension());
    server_config.setClientMaxBodySize(main_config.clientMaxBodySize());
    setErrorPageParams(server_config, main_config.errorPage());
    setIndexParams(server_config, main_config.index());
}

void ConfigParser::initLocationConfigFromServer(LocationConfig &location_config, const ServerConfig &server_config)
{
    setAllowMethodParams(location_config, server_config.allowMethod());
    location_config.setAutoindex(server_config.autoindex());
    setErrorPageParams(location_config, server_config.errorPage());
    setIndexParams(location_config, server_config.index());
    setReturnRedirectParam(location_config, server_config.returnRedirect());
}

const std::vector<std::string> ConfigParser::validateAllowMethodParams()
{
    std::vector<std::string> params;
    std::vector<std::string>::iterator iter = parse_line_.begin();

    ++iter;
    for (; (*iter != ";") && (iter != parse_line_.end()); ++iter)
    {
        params.push_back(*iter);
    }
    return params;
}

const std::map<int, std::string> ConfigParser::validateErrorPageParams()
{
    std::map<int, std::string> params;
    std::vector<std::string>::iterator status_code = parse_line_.begin();
    std::vector<std::string>::iterator uri = parse_line_.end();

    ++status_code;
    uri = uri - 2;
    for (; status_code != uri; ++status_code)
    {
        params.insert(std::make_pair(std::atoi(status_code->c_str()), *uri));
    }
    return params;
}


const std::vector<std::string> ConfigParser::validateIndexParams()
{
    std::vector<std::string> params;
    std::vector<std::string>::iterator iter = parse_line_.begin();

    ++iter;
    for (; (*iter != ";") && (iter != parse_line_.end()); ++iter)
    {
        params.push_back(*iter);
    }
    return params;
}

const std::map<int, std::string> ConfigParser::validateReturnParam()
{
    std::map<int, std::string> param;

    param.insert(std::make_pair(std::atoi(parse_line_[1].c_str()), parse_line_[2]));
    return param;
}
