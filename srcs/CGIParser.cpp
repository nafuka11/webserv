#include "CGIParser.hpp"
#include "HTTPParseException.hpp"
#include "SystemError.hpp"

const std::string CGIParser::NEWLINE = "\n";

CGIParser::CGIParser(HTTPRequest &request, const std::vector<ServerConfig> &configs, HTTPResponse &response)
    : HTTPParser(request, configs), response_(response)
{
}

CGIParser::~CGIParser()
{
}

void CGIParser::clear()
{
    headers_.clear();
    raw_message_.clear();
    parse_pos_ = 0;
    parse_state_ = PARSE_START_LINE;
    message_body_state_ = NONE;
    content_length_ = 0;
    chunk_size_ = 0;
}

void CGIParser::parse()
{
    std::string line;
    while (tryGetLine(line, NEWLINE))
    {
        if (line.empty())
        {
            break;
        }
        std::string name, value;
        splitHeader(line, name, value);
        headers_.insert(validateHeader(name, value));
    }
    if (!isValidHeaders())
    {
        throw HTTPParseException(CODE_502);
    }
    setInResponseHeaders();
    response_.setMessageBody(raw_message_.substr(parse_pos_, newline_pos_ - parse_pos_));
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
    if (headers_.count("content-type") == 0
        && headers_.count("location") == 0
        && headers_.count("status") == 0)
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

void CGIParser::setInResponseHeaders()
{
    if (headers_.count("content-type") == 1)
    {
        response_.setHeader(std::make_pair("Content-Type", headers_["content-type"]));
    }
    if (headers_.count("location") == 1)
    {
        response_.setHeader(std::make_pair("Location", headers_["location"]));
    }
    if (headers_.count("status") == 1)
    {
        response_.setHeader(std::make_pair("Status", headers_["status"]));
    }
}
