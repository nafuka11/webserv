#include "HTTPParser.hpp"
#include "HTTPParseException.hpp"
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cerrno>

const std::string HTTPParser::NEWLINE = "\r\n";

HTTPParser::HTTPParser(HTTPRequest &request, const std::vector<ServerConfig> &configs)
    : request_(request), configs_(configs), parse_pos_(0),
      parse_state_(PARSE_START_LINE), message_body_state_(NONE),
      content_length_(0), chunk_size_(0)
{
}

HTTPParser::~HTTPParser()
{
}

void HTTPParser::clear()
{
    raw_message_ = raw_message_.substr(parse_pos_);
    parse_pos_ = 0;
    parse_state_ = PARSE_START_LINE;
    message_body_state_ = NONE;
    content_length_ = 0;
    chunk_size_ = 0;
}

void HTTPParser::appendRawMessage(const char *message, size_t size)
{
    raw_message_.append(message, size);
}

bool HTTPParser::finished()
{
    return parse_state_ == PARSE_FINISH;
}

void HTTPParser::parse()
{
    bool need_parse = true;

    while (need_parse)
    {
        switch (parse_state_)
        {
        case PARSE_START_LINE:
            need_parse = parseStartLine();
            break;
        case PARSE_HEADERS:
            need_parse = parseHeader();
            break;
        case PARSE_MESSAGE_BODY:
            need_parse = parseMessageBody();
            break;
        case PARSE_FINISH:
            need_parse = false;
            break;
        }
    }
}

bool HTTPParser::parseStartLine()
{
    std::string line;
    if (!tryGetLine(line))
    {
        return false;
    }
    if (line.empty())
    {
        return false;
    }

    std::string method, uri, protocol_version;
    splitStartLine(line, method, uri, protocol_version);
    request_.setMethod(validateMethod(method));
    request_.setUri(validateUri(uri));
    request_.setProtocolVersion(validateProtocolVersion(protocol_version));
    parse_state_ = PARSE_HEADERS;
    return true;
}

bool HTTPParser::parseHeader()
{
    std::string line;
    if (!tryGetLine(line))
    {
        return false;
    }
    if (line.empty())
    {
        findServerConfig();
        findLocation();
        if (!isValidHeaders())
        {
            throw HTTPParseException(CODE_400);
        }
        if (!isAllowMethod(request_.getMethod()))
        {
            throw HTTPParseException(CODE_405);
        }
        message_body_state_ = judgeParseMessageBodyState();
        if (message_body_state_ == NONE)
        {
            parse_state_ = PARSE_FINISH;
        }
        else
        {
            parse_state_ = PARSE_MESSAGE_BODY;
        }
        return true;
    }
    std::string name, value;
    splitHeader(line, name, value);
    request_.setHeader(validateHeader(name, value));
    return true;
}

bool HTTPParser::parseMessageBody()
{
    switch (message_body_state_)
    {
    case CONTENT_LENGTH:
        return parseMessageBodyFromContentLength();
    case CHUNK_SIZE:
        return parseMessageBodyFromChunkSize();
    case CHUNK_DATA:
        return parseMessageBodyFromChunkData();
    default:
        parse_state_ = PARSE_FINISH;
        break;
    }
    return true;
}

bool HTTPParser::parseMessageBodyFromContentLength()
{
    if (raw_message_.size() - parse_pos_ < content_length_)
    {
        return false;
    }
    std::string message_body = raw_message_.substr(parse_pos_, content_length_);
    request_.setMessageBody(message_body);
    parse_pos_ += content_length_;
    parse_state_ = PARSE_FINISH;
    return true;
}

bool HTTPParser::parseMessageBodyFromChunkSize()
{
    std::string line;
    if (!tryGetLine(line))
    {
        return false;
    }
    // ;で分割した最初の文字列を得る
    size_t chunk_ext_index = line.find(";");
    if (chunk_ext_index != std::string::npos)
    {
        line = line.substr(0, chunk_ext_index);
    }
    // 文字列から末尾のスペースを除去する
    size_t space_index = line.find_last_not_of(" \t");
    if (space_index != std::string::npos)
    {
        line = line.substr(0, space_index + 1);
    }
    // 16進数でstrtolした物をchunk_sizeとして扱う
    setChunkSize(convertMessageBodySize(line, 16));

    if (chunk_size_ == 0)
    {
        parse_state_ = PARSE_FINISH;
    }
    else
    {
        message_body_state_ = CHUNK_DATA;
    }
    return true;
}

bool HTTPParser::parseMessageBodyFromChunkData()
{
    if (raw_message_.size() - parse_pos_ < chunk_size_ + NEWLINE.size())
    {
        return false;
    }
    if (!(raw_message_.at(parse_pos_ + chunk_size_) == '\r' &&
          raw_message_.at(parse_pos_ + chunk_size_ + 1) == '\n'))
    {
        throw HTTPParseException(CODE_400);
    }
    std::string message_body = raw_message_.substr(parse_pos_, chunk_size_);
    request_.appendMessageBody(message_body);
    parse_pos_ += chunk_size_ + NEWLINE.size();
    message_body_state_ = CHUNK_SIZE;
    return true;
}

HTTPParser::MessageBodyState HTTPParser::judgeParseMessageBodyState()
{
    const std::map<std::string, std::string> headers = request_.getHeaders();
    bool has_content_length = (headers.count("content-length") > 0);
    bool has_transfer_encoding = (headers.count("transfer-encoding") > 0);

    if (request_.getMethod() != HTTPRequest::HTTP_POST ||
        (!has_content_length && !has_transfer_encoding))
    {
        return HTTPParser::NONE;
    }
    if (has_content_length && has_transfer_encoding)
    {
        throw HTTPParseException(CODE_400);
    }
    if (has_content_length)
    {
        return HTTPParser::CONTENT_LENGTH;
    }
    if (headers.find("transfer-encoding")->second == "chunked")
    {
        return HTTPParser::CHUNK_SIZE;
    }
    else
    {
        throw HTTPParseException(CODE_400);
    }
}

void HTTPParser::findServerConfig()
{
    std::map<std::string, std::string>::const_iterator
        host = request_.getHeaders().find("host");
    if (host != request_.getHeaders().end())
    {
        for (size_t i = 0; i < configs_.size(); i++)
        {
            if (configs_.at(i).serverName() == host->second)
            {
                request_.setServerConfig(&configs_.at(i));
                return;
            }
        }
    }
    else
    {
        throw HTTPParseException(CODE_400);
    }
    request_.setServerConfig(&configs_.at(0));
}

void HTTPParser::findLocation()
{
    const std::string uri = request_.getUri();
    const std::map<std::string, LocationConfig> &
        locations = request_.getServerConfig()->location();
    std::map<std::string, LocationConfig>::const_reverse_iterator
        riter = locations.rbegin();

    for (; riter != locations.rend(); ++riter)
    {
        if (startsWith(uri, riter->first))
        {
            request_.setLocation(riter->first);
            return;
        }
    }
    throw HTTPParseException(CODE_404);
}

bool HTTPParser::isAllowMethod(const std::string &method)
{
    const std::map<std::string, LocationConfig> &
        locations = request_.getServerConfig()->location();
    std::map<std::string, LocationConfig>::const_iterator
        location_found = locations.find(request_.getLocation());
    if (location_found == locations.end())
    {
        throw HTTPParseException(CODE_404);
    }
    const std::vector<std::string> &allow_methods = location_found->second.allowMethod();

    const std::vector<std::string>::const_iterator
        method_found = std::find(allow_methods.begin(), allow_methods.end(), method);
    if (method_found == allow_methods.end())
    {
        return false;
    }
    return true;
}

bool HTTPParser::tryGetLine(std::string &line)
{
    size_t newline_pos = raw_message_.find(NEWLINE, parse_pos_);
    if (newline_pos == std::string::npos)
    {
        return false;
    }
    line = raw_message_.substr(parse_pos_, newline_pos - parse_pos_);
    parse_pos_ = newline_pos + NEWLINE.size();
    return true;
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

const std::string &HTTPParser::validateMethod(const std::string &method)
{
    if (method == HTTPRequest::HTTP_GET)
    {
        return method;
    }
    if (method == HTTPRequest::HTTP_POST)
    {
        return method;
    }
    if (method == HTTPRequest::HTTP_DELETE)
    {
        return method;
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
        setContentLength(convertMessageBodySize(value, 10));
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
    if (headers.count("content-length"))
    {
        int client_max_body_size = request_.getServerConfig()->clientMaxBodySize();
        if (client_max_body_size != 0 &&
            content_length_ > static_cast<size_t>(client_max_body_size))
        {
            throw HTTPParseException(CODE_413);
        }
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

size_t HTTPParser::convertMessageBodySize(const std::string &value, int radix)
{
    char *endp = NULL;
    long size = strtol(value.c_str(), &endp, radix);
    if (*endp != '\0' || size < 0)
    {
        throw HTTPParseException(CODE_400);
    }
    if ((size == LONG_MAX || size == LONG_MIN) && errno == ERANGE)
    {
        throw HTTPParseException(CODE_413);
    }
    return size;
}

void HTTPParser::setContentLength(size_t content_length)
{
    content_length_ = content_length;
}

void HTTPParser::setChunkSize(size_t chunk_size)
{
    int client_max_body_size = request_.getServerConfig()->clientMaxBodySize();
    if (client_max_body_size != 0 &&
        chunk_size + request_.getMessageBody().size() > static_cast<size_t>(client_max_body_size))
    {
        throw HTTPParseException(CODE_413);
    }
    chunk_size_ = chunk_size;
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

bool HTTPParser::startsWith(const std::string &str, const std::string &prefix) const
{
    return str.size() >= prefix.size() &&
           std::equal(std::begin(prefix), std::end(prefix), std::begin(str));
}
