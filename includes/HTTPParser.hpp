#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include "HTTPRequest.hpp"
#include "ServerConfig.hpp"
#include <vector>

class HTTPParser
{
public:
    enum ParseState
    {
        PARSE_START_LINE,
        PARSE_HEADERS,
        PARSE_MESSAGE_BODY,
        PARSE_FINISH
    };

    HTTPParser(HTTPRequest &request, const ServerConfig &config);
    ~HTTPParser();
    void clear();
    void parse();
    void appendRawMessage(const char *message);
    bool finished();

private:
    HTTPRequest &request_;
    const ServerConfig &config_;
    std::string raw_message_;
    size_t parse_pos_;
    ParseState state_;

    void parseLine(const std::string &line);
    void parseStartLine(const std::string &line);
    void parseHeader(const std::string &line);
    void parseMessageBody(const std::string &line);
    bool needsParsingMessageBody();

    void splitStartLine(const std::string &line,
                        std::string &method, std::string &uri,
                        std::string &protocol_version);
    void splitHeader(const std::string &line,
                     std::string &header_name, std::string &header_value);
    std::vector<std::string> splitString(const std::string &str,
                                         const std::string &delim);

    HTTPMethod validateMethod(const std::string &method);
    const std::string &validateUri(const std::string &uri);
    const std::string &validateProtocolVersion(const std::string &protocol_version);
    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    const std::string &validateHeaderName(std::string &name);
    const std::string &validateHeaderValue(const std::string &value);
    bool isValidHeaders();

    bool isSpace(char c);
    bool isToken(const std::string &str);
    bool isTokenChar(char c);
};

#endif /* HTTPPARSER_HPP */
