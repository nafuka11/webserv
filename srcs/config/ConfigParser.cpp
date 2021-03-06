#include "ConfigParser.hpp"
#include <fstream>
#include <string>
#include "Config.hpp"
#include "ConfigConstant.hpp"
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
const int ConfigParser::SERVER_OPEN_BRACE_INDEX = 1;
const int ConfigParser::LOCATION_OPEN_BRACE_INDEX = 2;
const int ConfigParser::PORT_MAX_VALUE = 65535;
const int ConfigParser::PORT_MIN_VALUE = 1;
const int ConfigParser::ERROR_PAGE_MAX_STATUS_CODE = 599;
const int ConfigParser::ERROR_PAGE_MIN_STATUS_CODE = 300;
const int ConfigParser::RETURN_REDIRECT_MAX_STATUS_CODE = 999;
const int ConfigParser::RETURN_REDIRECT_MIN_STATUS_CODE = 0;


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
    ifs.close();
    parseMainContext();
}

void ConfigParser::readAndSplitLines(std::ifstream &ifs)
{
    std::string line;

    while (std::getline(ifs, line))
    {
        // skip comment out
        if (line.size() != 0 && line[0] == '#')
        {
            continue;
        }
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
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
        if (directive_type_ == SERVER)
        {
            setDefaultToUnsetMainValue(main_config);
        }

        std::map<DirectiveType, main_parse_func>::const_iterator miter;
        miter = MAIN_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(main_config);
    }
}

void ConfigParser::parseServerContext(MainConfig &main_config)
{
    validateStartServerContext();

    ServerConfig server_config = ServerConfig();
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
        if (parse_line_[0] == "}")
        {
            break ;
        }
        setDirectiveType(parse_line_[DIRECTIVE_NAME_INDEX]);
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
        if (directive_type_ == LOCATION)
        {
            setDefaultToUnsetServerValue(server_config, main_config);
        }

        std::map<DirectiveType, server_parse_func>::const_iterator miter;
        miter = SERVER_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(server_config);
    }
    validateEndContext();
    config_.addServer(server_config);
    setContextType(CONTEXT_MAIN);
}

void ConfigParser::parseLocationContext(ServerConfig &server_config)
{
    validateStartLocationContext();

    LocationConfig location_config = LocationConfig();
    std::string location_path = parse_line_[DIRECTIVE_VALUE_INDEX];

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
        if (parse_line_[0] == "}")
        {
            break ;
        }
        setDirectiveType(parse_line_[DIRECTIVE_NAME_INDEX]);
        if (!isAllowedDirective())
        {
            throw ConfigError(NOT_ALLOWED_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }

        std::map<DirectiveType, location_parse_func>::const_iterator miter;
        miter = LOCATION_PARSE_FUNC.find(directive_type_);
        (this->*miter->second)(location_config);
    }
    validateEndContext();
    if (isDuplicateLocation(server_config, location_path))
    {
        throw ConfigError(DUPLICATE_LOCATION, location_path, filepath_, (line_pos_ + 1));
    }
    setDefaultToUnsetLocationValue(location_config, server_config);
    server_config.addLocation(location_path, location_config);
    setContextType(CONTEXT_SERVER);
}

void ConfigParser::parseAlias(LocationConfig &location_config)
{
    validateDuplicateValueTypeStr(location_config.alias());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();
    location_config.setAlias(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

void ConfigParser::parseListen(ServerConfig &server_config)
{
    validateDuplicateValueTypeInt(server_config.listen());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();

    long value = convertNumber(parse_line_[DIRECTIVE_VALUE_INDEX]);
    if (value < PORT_MIN_VALUE || value > PORT_MAX_VALUE)
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    server_config.setListen(value);
}

void ConfigParser::parseServerName(ServerConfig &server_config)
{
    validateDuplicateValueTypeStr(server_config.serverName());
    validateNumOfArgs(NUM_ONE);
    validateEndSemicolon();
    server_config.setServerName(parse_line_[DIRECTIVE_VALUE_INDEX]);
}

void ConfigParser::validateEndContext()
{
    if (line_pos_ == parse_lines_.size())
    {
        throw ConfigError(UNEXPECTED_END, "", filepath_, (line_pos_ + 1));
    }
    if (parse_line_.size() == 1 && parse_line_[0] == "}")
    {
        return;
    }
    throw ConfigError(UNEXPECTED, parse_line_[1], filepath_, (line_pos_ + 1));
}

void ConfigParser::validateEndSemicolon()
{
    std::vector<std::string>::iterator iter =
        std::find(parse_line_.begin(), parse_line_.end(), ";");
    if (iter == parse_line_.end())
    {
        throw ConfigError(NO_END_SEMICOLON, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    if (iter + 1 == parse_line_.end())
    {
        return ;
    }
    throw ConfigError(UNEXPECTED, *iter, filepath_, (line_pos_ + 1));
}

void ConfigParser::validateNumOfArgs(DirectiveNumArgs num)
{
    std::vector<std::string>::const_iterator value = parse_line_.begin();
    int count = 0;

    for (++value; (value != parse_line_.end() && (*value != ";")); ++value)
    {
        count++;
    }
    switch (num)
    {
    case NUM_ONE:
    case NUM_TWO:
        if (count != num)
        {
            throw ConfigError(INVALID_NUM_OF_ARGS, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
        break;
    case NUM_MULTIPLE:
        if (count == 0)
        {
            throw ConfigError(INVALID_NUM_OF_ARGS, parse_line_[DIRECTIVE_NAME_INDEX],
                              filepath_, (line_pos_ + 1));
        }
        break;
    default:
        break;
    }
}

void ConfigParser::validateStartServerContext()
{
    std::vector<std::string>::iterator iter =
        std::find(parse_line_.begin(), parse_line_.end(), "{");
    if (iter == parse_line_.end())
    {
        throw ConfigError(NO_OPEN_DIRECTIVE, "server", filepath_, (line_pos_ + 1));
    }

    if (parse_line_.size() == 2)
    {
        return;
    }

    size_t open_brace_index = std::distance(parse_line_.begin(), iter);
    if (open_brace_index == SERVER_OPEN_BRACE_INDEX)
    {
        throw ConfigError(UNEXPECTED, parse_line_[open_brace_index + 1],
                          filepath_, (line_pos_ + 1));
    }
    throw ConfigError(UNEXPECTED, parse_line_[SERVER_OPEN_BRACE_INDEX],
                      filepath_, (line_pos_ + 1));
}

void ConfigParser::validateStartLocationContext()
{
    std::vector<std::string>::iterator iter =
        std::find(parse_line_.begin(), parse_line_.end(), "{");
    if (iter == parse_line_.end())
    {
        throw ConfigError(NO_OPEN_DIRECTIVE, "location", filepath_, (line_pos_ + 1));
    }

    size_t open_brace_index = std::distance(parse_line_.begin(), iter);
    if (parse_line_.size() == 3 && open_brace_index == LOCATION_OPEN_BRACE_INDEX)
    {
        return;
    }
    if (open_brace_index == 1 || (open_brace_index + 1) == parse_line_.size())
    {
        throw ConfigError(INVALID_NUM_OF_ARGS, "location", filepath_, (line_pos_ + 1));
    }
    throw ConfigError(UNEXPECTED, parse_line_[open_brace_index + 1],
                      filepath_, (line_pos_ + 1));
}

void ConfigParser::validateDuplicateValueTypeInt(const int value)
{
    if (value != ConfigConstant::UNSET_TYPE_INT)
    {
        throw ConfigError(DUPLICATE_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
}

void ConfigParser::validateDuplicateValueTypeStr(const std::string &value)
{
    if (value != ConfigConstant::UNSET_TYPE_STR)
    {
        throw ConfigError(DUPLICATE_DIRECTIVE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
}

void ConfigParser::validateContainsValues(std::vector<std::string> &values,
                                          const std::vector<std::string> &set_values)
{
    std::vector<std::string>::iterator value = parse_line_.begin();

    ++value;
    for (; (*value != ";") && (value != parse_line_.end()); ++value)
    {
        if (containsValue(*value, set_values))
        {
            throw ConfigError(DUPLICATE_VALUE,
                              parse_line_[DIRECTIVE_NAME_INDEX] + ":" + *value,
                              filepath_, (line_pos_ + 1));
        }
        values.push_back(*value);
    }
}

bool ConfigParser::containsValue(std::string &value,
                                 const std::vector<std::string> &set_values)
{
    std::vector<std::string>::const_iterator found;

    found = std::find(set_values.begin(), set_values.end(), value);
    if (found != set_values.end())
    {
        return true;
    }
    return false;
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

bool ConfigParser::isDuplicateLocation(const ServerConfig &server_config,
                                       const std::string &path)
{
    std::map<std::string, LocationConfig> location = server_config.location();
    std::map<std::string, LocationConfig>::iterator iter = location.find(path);

    if (iter != location.end())
    {
        return true;
    }
    return false;
}

std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> >
    ConfigParser::createAllowedDirective()
{
    std::map<ConfigParser::DirectiveType, std::vector<ConfigParser::ContextType> >
        allowed_directive;

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

long ConfigParser::convertNumber(const std::string &str)
{
    char *endp = NULL;
    long value = strtol(str.c_str(), &endp, 10);

    if (*endp != '\0')
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    if ((value == LONG_MAX || value == LONG_MIN) && ERANGE == errno)
    {
        throw ConfigError(INVALID_VALUE, parse_line_[DIRECTIVE_NAME_INDEX],
                          filepath_, (line_pos_ + 1));
    }
    return value;
}

std::map<ConfigParser::DirectiveType, ConfigParser::main_parse_func>
    ConfigParser::createMainParseFunc()
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

std::map<ConfigParser::DirectiveType, ConfigParser::server_parse_func>
    ConfigParser::createServerParseFunc()
{
    std::map<ConfigParser::DirectiveType, server_parse_func> parse_func;

    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] =  &ConfigParser::parseAutoindex;
    parse_func[CGI_EXTENSION] = &ConfigParser::parseCgiExtension;
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

std::map<ConfigParser::DirectiveType, ConfigParser::location_parse_func>
    ConfigParser::createLocationParseFunc()
{
    std::map<ConfigParser::DirectiveType, location_parse_func> parse_func;

    parse_func[ALIAS] = &ConfigParser::parseAlias;
    parse_func[ALLOW_METHOD] = &ConfigParser::parseAllowMethod;
    parse_func[AUTOINDEX] = &ConfigParser::parseAutoindex;
    parse_func[CGI_EXTENSION] = &ConfigParser::parseCgiExtension;
    parse_func[ERROR_PAGE] = &ConfigParser::parseErrorPage;
    parse_func[INDEX] = &ConfigParser::parseIndex;
    parse_func[RETURN] = &ConfigParser::parseReturnRedirect;
    parse_func[UPLOAD_PATH] = &ConfigParser::parseUploadPath;
    return parse_func;
}

std::vector<ConfigParser::ContextType>
    ConfigParser::generateAllowedContext(DirectiveType state)
{
    std::vector<ContextType> allowed_context;

    switch (state)
    {
    case ALLOW_METHOD:
    case AUTOINDEX:
    case CGI_EXTENSION:
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
        throw ConfigError(UNKOWN_DIRECTIVE, directive_name, filepath_, (line_pos_ + 1));
}

void ConfigParser::setDefaultToUnsetMainValue(MainConfig &main_config)
{
    if (main_config.allowMethod().empty())
    {
        setAllowMethod(main_config, ConfigConstant::DEFAULT_ALLOW_METHOD);
    }
    if  (main_config.autoindex() == ConfigConstant::UNSET_TYPE_STR)
    {
        main_config.setAutoindex(ConfigConstant::DEFAULT_AUTOINDEX);
    }
    if (main_config.clientMaxBodySize() == ConfigConstant::UNSET_TYPE_INT)
    {
        main_config.setClientMaxBodySize(ConfigConstant::DEFAULT_CLIENT_MAX_BODY_SIZE);
    }
    if (main_config.index().empty())
    {
        setIndex(main_config, ConfigConstant::DEFAULT_INDEX);
    }
}

void ConfigParser::setDefaultToUnsetServerValue(ServerConfig &server_config,
                                                const MainConfig &main_config)
{
    if (server_config.allowMethod().empty())
    {
        setAllowMethod(server_config, main_config.allowMethod());
    }
    if  (server_config.autoindex() == ConfigConstant::UNSET_TYPE_STR)
    {
        server_config.setAutoindex(main_config.autoindex());
    }
    if (server_config.cgiExtension().empty())
    {
        setCgiExtension(server_config, main_config.cgiExtension());
    }
    if (server_config.clientMaxBodySize() == ConfigConstant::UNSET_TYPE_INT)
    {
        server_config.setClientMaxBodySize(main_config.clientMaxBodySize());
    }
    if (server_config.errorPage().empty() && !main_config.errorPage().empty())
    {
        setErrorPage(server_config, main_config.errorPage());
    }
    if (server_config.index().empty())
    {
        setIndex(server_config, main_config.index());
    }
    if (server_config.listen() == ConfigConstant::UNSET_TYPE_INT)
    {
        server_config.setListen(ConfigConstant::DEFAULT_PORT);
    }
}

void ConfigParser::setDefaultToUnsetLocationValue(LocationConfig &location_config,
                                                  const ServerConfig &server_config)
{
    if (location_config.allowMethod().empty())
    {
        setAllowMethod(location_config, server_config.allowMethod());
    }
    if (location_config.autoindex() == ConfigConstant::UNSET_TYPE_STR)
    {
        location_config.setAutoindex(server_config.autoindex());
    }
    if (location_config.cgiExtension().empty())
    {
        setCgiExtension(location_config, server_config.cgiExtension());
    }
    if (location_config.errorPage().empty())
    {
        setErrorPage(location_config, server_config.errorPage());
    }
    if (location_config.index().empty())
    {
        setIndex(location_config, server_config.index());
    }
    if (location_config.returnRedirect().empty() && !server_config.returnRedirect().empty())
    {
        setReturnRedirect(location_config, server_config.returnRedirect());
    }
    if (location_config.uploadPath() == ConfigConstant::UNSET_TYPE_STR
        && server_config.uploadPath() != ConfigConstant::UNSET_TYPE_STR)
    {
        location_config.setUploadPath(server_config.uploadPath());
    }
}
