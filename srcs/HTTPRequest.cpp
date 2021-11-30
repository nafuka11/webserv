#include "HTTPRequest.hpp"
#include "HTTPParseException.hpp"

HTTPRequest::HTTPRequest() : parse_pos_(0), state_(PARSE_START_LINE)
{
}

HTTPRequest::~HTTPRequest()
{
}

void HTTPRequest::clear()
{
    raw_message_.clear();
    parse_pos_ = 0;
    state_ = PARSE_START_LINE;
    message_body_.clear();
}

void HTTPRequest::appendRawMessage(const char *message)
{
    raw_message_.append(message);
}

void HTTPRequest::parse()
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
            std::cout << "method: \"" << method_ << "\"" << std::endl
                    << "uri: \"" << uri_ << "\"" << std::endl
                    << "protocol: \"" << protocol_version_ << "\"" << std::endl;
            for (std::map<std::string, std::string>::iterator iter = headers_.begin();
                iter != headers_.end();
                ++iter)
            {
                std::cout << (*iter).first << ": " << (*iter).second << std::endl;
            }
            clear();
            break;
        }
        parse_pos_ = newline_pos + 2;
    }
}

void HTTPRequest::parseLine(const std::string &line)
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

void HTTPRequest::parseStartLine(const std::string &line)
{
    if (line.empty())
    {
        return;
    }

    std::vector<std::string> items = split(line, " ");
    if (items.size() != 3)
    {
        throw HTTPParseException(CODE_400);
    }
    setMethod(items[0]);
    setUri(items[1]);
    setProtocolVersion(items[2]);
    state_ = PARSE_HEADERS;
}

void HTTPRequest::parseHeader(const std::string &line)
{
    if (line.empty())
    {
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
    size_t delim_pos = line.find(":", 0);
    if (delim_pos == std::string::npos)
    {
        throw HTTPParseException(CODE_400);
    }
    std::string key = line.substr(0, delim_pos);
    size_t value_pos = delim_pos + 1;
    while (isSpace(line[value_pos]))
    {
        value_pos++;
    }
    std::string value = line.substr(value_pos, line.size() - value_pos);
    setHeader(key, value);
}

void HTTPRequest::parseMessageBody(const std::string &line)
{
    (void)line;
    state_ = PARSE_FINISH;
}

bool HTTPRequest::needsParsingMessageBody()
{
    return method_ == POST &&
        (headers_.count("Content-Length") || headers_.count("Transfer-Encoding"));
}

bool HTTPRequest::isSpace(char c)
{
    return c == ' ';
}

std::vector<std::string> HTTPRequest::split(const std::string &str, const std::string &delim)
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delim, start)) != std::string::npos)
    {
        words.push_back(str.substr(start, end - start));
        start = end + delim.size();
    }
    if (start < str.size())
    {
        words.push_back(str.substr(start, str.size() - start));
    }
    return words;
}

void HTTPRequest::setMethod(const std::string &method)
{
    if (method == "GET")
    {
        method_ = GET;
    }
    else if (method == "POST")
    {
        method_ = POST;
    }
    else if (method == "DELETE")
    {
        method_ = DELETE;
    }
    else
    {
        throw HTTPParseException(CODE_501);
    }
}
void HTTPRequest::setUri(const std::string &uri)
{
    uri_ = uri;
}

void HTTPRequest::setProtocolVersion(const std::string &protocol_version)
{
    if (protocol_version != "HTTP/1.1")
    {
        throw HTTPParseException(CODE_400);
    }
    protocol_version_ = protocol_version;
}

void HTTPRequest::setHeader(const std::string &key, const std::string &value)
{
    headers_.insert(std::make_pair(key, value));
}

void HTTPRequest::setMessageBody(const std::string &message_body)
{
    (void)message_body;
}
