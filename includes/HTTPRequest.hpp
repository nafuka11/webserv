#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "HTTPMethod.hpp"

class HTTPRequest
{
public:
    enum ParseState
    {
        PARSE_START_LINE,
        PARSE_HEADERS,
        PARSE_MESSAGE_BODY,
        PARSE_FINISH
    };

    HTTPRequest();
    ~HTTPRequest();
    void clear();
    void parse();
    void appendRawMessage(const char *message);

private:
    std::string raw_message_;
    size_t parse_pos_;
    ParseState state_;

    HTTPMethod method_;
    std::string uri_;
    std::string protocol_version_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;

    void parseLine(const std::string &line);
    void parseStartLine(const std::string &line);
    void parseHeader(const std::string &line);
    void parseMessageBody(const std::string &line);
    bool needsParsingMessageBody();
    bool isSpace(char c);
    bool isToken(const std::string &str);
    bool isTokenChar(char c);
    std::vector<std::string> split(const std::string &str, const std::string &delim);

    void setMethod(const std::string &method);
    void setUri(const std::string &uri);
    void setProtocolVersion(const std::string &protocol_version);
    void setHeader(const std::string &key, const std::string &value);
    void setMessageBody(const std::string &message_body);
};

#endif /* HTTPREQUEST_HPP */
