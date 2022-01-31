#include "CGIParser.hpp"

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
    std::string line;
    while (tryGetLine(line))
    {
        if (line.empty())
        {
            break;
        }
        std::string name, value;
        splitHeader(line, name, value);
        response.setHeader(validateHeader(name, value));
    }
    response.setMessageBody(raw_message_.substr(parse_pos_, newline_pos_ - parse_pos_));
}

const std::pair<std::string, std::string> CGIParser::validateHeader(std::string &name,
                                                                    std::string &value)
{
    name = validateHeaderName(name);
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
