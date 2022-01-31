#include "CGIParser.hpp"
#include "HTTPParseException.hpp"
#include <iostream>　//TODO:  後で消す

const std::string CGIParser::NEWLINE = "\n";

CGIParser::CGIParser(HTTPRequest &request, const ServerConfig &config)
    : HTTPParser(request, config)
{
}

CGIParser::~CGIParser()
{
}

void CGIParser::parse(HTTPResponse &response)
{
    std::cout << "\nCalled CGIParser::parse" << std::endl; //TODO:  後で消す
    std::string line;
    while (tryGetLine(line, NEWLINE))
    {
        std::cout << "line: " << line; //TODO:  後で消す
        if (line.empty())
        {
            break;
        }
        std::string name, value;
        splitHeader(line, name, value);
        response.setHeader(validateHeader(name, value));
        std::cout << " name: " << name << " value: " << value << std::endl; //TODO: 後で消す
    }
    if (!isValidHeaders(response.getHeaders()))
    {
        throw HTTPParseException(CODE_502);
    }
    response.setMessageBody(raw_message_.substr(parse_pos_, newline_pos_ - parse_pos_));
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

bool CGIParser::isValidHeaders(const std::map<std::string, std::string> &headers)
{
    std::cout << "Called CGIParser::isValidHeaders: "; //TODO:  後で消す
    if (headers.count("content-type") == 0
        && headers.count("location") == 0
        && headers.count("status") == 0)
    {
        std::cout << "false" << std::endl; //TODO: 後で消す
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
        validateStatus();
    }
    return std::make_pair(name, value);
}

void CGIParser::validateStatus()
{
    // statusが3桁の整数値かチェック。
        // if (statusがエラー値)
            // HTTPParseException(502);
}
