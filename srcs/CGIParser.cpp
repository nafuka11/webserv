#include "CGIParser.hpp"
#include "HTTPParseException.hpp"
#include "SystemError.hpp"

const std::string CGIParser::NEWLINE = "\n";

CGIParser::CGIParser(HTTPRequest &request, const std::vector<ServerConfig> &configs, HTTPResponse &response)
    : HTTPParser(request, configs), response_(response)
{
    parse_state_ = PARSE_HEADERS;
}

CGIParser::~CGIParser()
{
}

void CGIParser::clear()
{
    raw_cgi_headers_.clear();
    raw_message_.clear();
    parse_pos_ = 0;
    parse_state_ = PARSE_HEADERS;
    message_body_state_ = NONE;
    content_length_ = 0;
    chunk_size_ = 0;
}

void CGIParser::parse()
{
    bool need_parse = true;

    while (need_parse)
    {
        switch (parse_state_)
        {
        case PARSE_HEADERS:
            need_parse = parseHeader();
            break;
        case PARSE_MESSAGE_BODY:
            need_parse = parseMessageBody();
            break;
        case PARSE_FINISH:
            need_parse = false;
            break;
        default:
            break;
        }
    }
}

bool CGIParser::parseHeader()
{
    std::string line;
    if (!tryGetLine(line, NEWLINE))
    {
        return false;
    }
    if (line.empty())
    {
        if (!isValidHeaders())
        {
            throw HTTPParseException(CODE_502);
        }
        generateResponseHeaders();
        parse_state_ = PARSE_MESSAGE_BODY;
        return true;
    }
    std::string name, value;
    splitHeader(line, name, value);
    raw_cgi_headers_.insert(validateHeader(name, value));
    return true;
}

bool CGIParser::parseMessageBody()
{
    std::string message_body = raw_message_.substr(parse_pos_);
    response_.appendMessageBody(message_body.c_str(), message_body.size());
    parse_pos_ += content_length_;
    parse_state_ = PARSE_FINISH;
    return true;
}


void CGIParser::splitHeader(const std::string &line,
                            std::string &header_name, std::string &header_value)
{
    size_t delim_pos = line.find(":", 0);
    if (delim_pos == std::string::npos)
    {
        throw HTTPParseException(CODE_502);
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

bool CGIParser::isValidHeaders()
{
    if (raw_cgi_headers_.count("content-type") == 0
        && raw_cgi_headers_.count("location") == 0
        && raw_cgi_headers_.count("status") == 0)
    {
        return false;
    }
    return true;
}

const std::pair<std::string, std::string> CGIParser::validateHeader(std::string &name,
                                                                    std::string &value)
{
    name = validateHeaderName(name);
    value = validateHeaderValue(value);
    if (name == "status")
    {
        validateStatus(value);
    }
    if (name == "location")
    {
        response_.setStatusCode(302);
    }
    return std::make_pair(name, value);
}

void CGIParser::validateStatus(const std::string &value)
{
    std::string::size_type found = value.find(' ');
    if (found == std::string::npos)
    {
        throw HTTPParseException(CODE_502);
    }

    std::string status_value = value.substr(0, found);
    char *endp = NULL;
    long status = strtol(status_value.c_str(), &endp, 10);
    if (*endp != '\0' || status < 0)
    {
        throw HTTPParseException(CODE_502);
    }
    if ((status == LONG_MAX || status == LONG_MIN) && errno == ERANGE)
    {
        throw HTTPParseException(CODE_502);
    }
    if (status < 100 || status > 999)
    {
        throw HTTPParseException(CODE_502);
    }
    response_.setStatusCode(status);
}

void CGIParser::generateResponseHeaders()
{
    if (raw_cgi_headers_.count("content-type") == 1)
    {
        response_.setHeader(std::make_pair("Content-Type", raw_cgi_headers_["content-type"]));
    }
    if (raw_cgi_headers_.count("location") == 1)
    {
        response_.setHeader(std::make_pair("Location", raw_cgi_headers_["location"]));
    }
    if (raw_cgi_headers_.count("status") == 1)
    {
        response_.setHeader(std::make_pair("Status", raw_cgi_headers_["status"]));
    }
}
