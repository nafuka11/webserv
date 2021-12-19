#include "HTTPParser.hpp"
#include "HTTPParseException.hpp"
#include <algorithm>
#include <cstring>
#include <cstdlib>

HTTPParser::HTTPParser(HTTPRequest &request, const ServerConfig &config)
    : request_(request), config_(config), parse_pos_(0), state_(PARSE_START_LINE)
{
}

HTTPParser::~HTTPParser()
{
}

void HTTPParser::clear()
{
    raw_message_.clear();
    parse_pos_ = 0;
    state_ = PARSE_START_LINE;
}

void HTTPParser::appendRawMessage(const char *message)
{
    raw_message_.append(message);
}

bool HTTPParser::finished()
{
    return state_ == PARSE_FINISH;
}

void HTTPParser::parse()
{
    size_t newline_pos;

    while (true)
    {
        newline_pos = raw_message_.find("\r\n", parse_pos_);
        if (newline_pos == std::string::npos)
        {
            break;
        }
        std::string line = raw_message_.substr(parse_pos_, newline_pos - parse_pos_);
        parseLine(line);
        if (state_ == PARSE_FINISH)
        {
            break;
        }
        parse_pos_ = newline_pos + 2;
    }
}

void HTTPParser::parseLine(const std::string &line)
{
    switch (state_)
    {
    case PARSE_START_LINE:
        parseStartLine(line);
        break;
    case PARSE_HEADERS:
        parseHeader(line);
        break;
    case PARSE_MESSAGE_BODY:
        parseMessageBody(line);
        break;
    default:
        break;
    }
}

void HTTPParser::parseStartLine(const std::string &line)
{
    if (line.empty())
    {
        return;
    }

    std::string method, uri, protocol_version;
    splitStartLine(line, method, uri, protocol_version);
    request_.setMethod(validateMethod(method));
    request_.setUri(validateUri(uri));
    request_.setProtocolVersion(validateProtocolVersion(protocol_version));
    state_ = PARSE_HEADERS;
}

void HTTPParser::parseHeader(const std::string &line)
{
    if (line.empty())
    {
        if (!isValidHeaders())
        {
            throw HTTPParseException(CODE_400);
        }
        if (needsParsingMessageBody())
        {
            state_ = PARSE_MESSAGE_BODY;
        }
        else
        {
            state_ = PARSE_FINISH;
        }
        return;
    }
    std::string name, value;
    splitHeader(line, name, value);
    request_.setHeader(validateHeader(name, value));
}

void HTTPParser::parseMessageBody(const std::string &line)
{
    request_.setMessageBody(line);
    state_ = PARSE_FINISH;
}

bool HTTPParser::needsParsingMessageBody()
{
    const std::map<std::string, std::string> headers = request_.getHeaders();
    return request_.getMethod() == POST &&
           (headers.count("content-length") || headers.count("transfer-encoding"));
}

void HTTPParser::splitStartLine(const std::string &line,
                                std::string &method, std::string &uri,
                                std::string &protocol_version)
{
    std::vector<std::string> items = splitString(line, " ");
    if (items.size() != 3)
    {
        throw HTTPParseException(CODE_400);
    }
    method = items[0];
    uri = items[1];
    protocol_version = items[2];
}

void HTTPParser::splitHeader(const std::string &line,
                             std::string &header_name, std::string &header_value)
{
    size_t delim_pos = line.find(":", 0);
    if (delim_pos == std::string::npos)
    {
        throw HTTPParseException(CODE_400);
    }
    header_name = line.substr(0, delim_pos);
    size_t value_start = delim_pos + 1;
    while (isSpace(line[value_start]))
    {
        value_start++;
    }
    size_t value_end = line.size();
    while (value_end - 1 > 0 && isSpace(line[value_end - 1]))
    {
        value_end--;
    }
    header_value = line.substr(value_start, value_end - value_start);
}

std::vector<std::string> HTTPParser::splitString(const std::string &str,
                                                 const std::string &delim)
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delim, start)) != std::string::npos)
    {
        words.push_back(str.substr(start, end - start));
        start = end + delim.size();
    }
    if (start <= str.size())
    {
        words.push_back(str.substr(start, str.size() - start));
    }
    return words;
}

HTTPMethod HTTPParser::validateMethod(const std::string &method)
{
    if (method == "GET")
    {
        return GET;
    }
    if (method == "POST")
    {
        return POST;
    }
    if (method == "DELETE")
    {
        return DELETE;
    }
    throw HTTPParseException(CODE_501);
}

const std::string &HTTPParser::validateUri(const std::string &uri)
{
    return uri;
}

const std::string &HTTPParser::validateProtocolVersion(
    const std::string &protocol_version)
{
    if (protocol_version != "HTTP/1.1")
    {
        throw HTTPParseException(CODE_400);
    }
    return protocol_version;
}

const std::pair<std::string, std::string> HTTPParser::validateHeader(std::string &name,
                                                                     std::string &value)
{
    name = validateHeaderName(name);
    value = validateHeaderValue(value);
    if (name == "host")
    {
        validateHost();
    }
    else if (name == "content-length")
    {
        validateContentLength(value);
    }
    return std::make_pair(name, value);
}

const std::string &HTTPParser::validateHeaderName(std::string &name)
{
    std::transform(name.begin(), name.end(), name.begin(), tolower);
    if (!isToken(name))
    {
        throw HTTPParseException(CODE_400);
    }
    return name;
}

const std::string &HTTPParser::validateHeaderValue(const std::string &value)
{
    if (value.empty())
    {
        throw HTTPParseException(CODE_400);
    }
    return value;
}

bool HTTPParser::isValidHeaders()
{
    const std::map<std::string, std::string> headers = request_.getHeaders();
    if (headers.count("host") == 0)
    {
        return false;
    }
    return true;
}

void HTTPParser::validateHost()
{
    if (request_.getHeaders().count("host") != 0)
    {
        throw HTTPParseException(CODE_400);
    }
}

void HTTPParser::validateContentLength(const std::string &value)
{
    char *endp = NULL;
    long length = strtol(value.c_str(), &endp, 10);
    if (*endp != '\0' || length < 0)
    {
        throw HTTPParseException(CODE_400);
    }
    if (config_.clientMaxBodySize() != 0 && length > config_.clientMaxBodySize())
    {
        throw HTTPParseException(CODE_413);
    }
    content_length_ = length;
}

bool HTTPParser::isSpace(char c)
{
    return c == ' ' || c == '\t';
}

bool HTTPParser::isToken(const std::string &str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!isTokenChar(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool HTTPParser::isTokenChar(char c)
{
    return isalnum(c) || strchr("!#$%&'*+-.^_`|~", c);
}
