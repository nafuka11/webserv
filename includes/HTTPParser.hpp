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

    enum MessageBodyState
    {
        CONTENT_LENGTH,
        CHUNK_SIZE,
        CHUNK_DATA,
        NONE
    };

    HTTPParser(HTTPRequest &request, const ServerConfig &config);
    ~HTTPParser();
    void clear();
    void parse();
    void appendRawMessage(const char *message);
    bool finished();

private:
    static const std::string NEWLINE;
    HTTPRequest &request_;
    const ServerConfig &config_;
    std::string raw_message_;
    size_t parse_pos_;
    ParseState parse_state_;
    MessageBodyState message_body_state_;
    size_t content_length_;
    size_t chunk_size_;

    bool parseStartLine();
    bool parseHeader();
    bool parseMessageBody();
    bool parseMessageBodyFromContentLength();
    bool parseMessageBodyFromChunkSize();
    bool parseMessageBodyFromChunkData();
    MessageBodyState judgeParseMessageBodyState();
    void findLocation();
    bool isAllowMethod(const std::string &method);

    bool tryGetLine(std::string &line);
    void splitStartLine(const std::string &line,
                        std::string &method, std::string &uri,
                        std::string &protocol_version);
    void splitHeader(const std::string &line,
                     std::string &header_name, std::string &header_value);
    std::vector<std::string> splitString(const std::string &str,
                                         const std::string &delim);

    const std::string &validateMethod(const std::string &method);
    const std::string &validateUri(const std::string &uri);
    const std::string &validateProtocolVersion(const std::string &protocol_version);
    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    const std::string &validateHeaderName(std::string &name);
    const std::string &validateHeaderValue(const std::string &value);
    bool isValidHeaders();
    void validateHost();
    size_t convertMessageBodySize(const std::string &value, int radix);

    void setContentLength(size_t content_length);
    void setChunkSize(size_t chunk_size);

    bool isSpace(char c);
    bool isToken(const std::string &str);
    bool isTokenChar(char c);
    bool startsWith(const std::string &str, const std::string &prefix) const;
};

#endif /* HTTPPARSER_HPP */
