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

    HTTPParser(HTTPRequest &request, const std::vector<ServerConfig> &configs);
    virtual ~HTTPParser();
    virtual void clear();
    virtual void parse();
    void appendRawMessage(const char *message, size_t size);
    bool finished();
    size_t getContentLength() const;

protected:
    std::string raw_message_;
    size_t parse_pos_;
    size_t newline_pos_;
    ParseState parse_state_;
    MessageBodyState message_body_state_;
    size_t content_length_;
    size_t chunk_size_;

    virtual bool parseHeader();
    virtual bool parseMessageBody();

    virtual void splitHeader(const std::string &line,
                             std::string &header_name, std::string &header_value);
    virtual bool isValidHeaders();
    virtual const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                                     std::string &value);
    bool tryGetLine(std::string &line, const std::string &newline);
    bool isSpace(char c);
    const std::string &validateHeaderName(std::string &name);
    const std::string &validateHeaderValue(const std::string &value);

private:
    static const std::string NEWLINE;
    HTTPRequest &request_;
    const std::vector<ServerConfig> &configs_;

    bool parseStartLine();
    bool parseMessageBodyFromContentLength();
    bool parseMessageBodyFromChunkSize();
    bool parseMessageBodyFromChunkData();
    MessageBodyState judgeParseMessageBodyState();
    void findServerConfig();
    void findLocation();
    bool isAllowMethod(const std::string &method);

    void splitStartLine(const std::string &line,
                        std::string &method, std::string &uri,
                        std::string &protocol_version);
    std::vector<std::string> splitString(const std::string &str,
                                         const std::string &delim);

    const std::string &validateMethod(const std::string &method);
    const std::string validateUri(const std::string &uri);
    const std::string &validateProtocolVersion(const std::string &protocol_version);
    void validateHost();
    size_t convertMessageBodySize(const std::string &value, int radix);

    void setContentLength(size_t content_length);
    void setChunkSize(size_t chunk_size);

    bool isToken(const std::string &str);
    bool isTokenChar(char c);
    bool startsWith(const std::string &str, const std::string &prefix) const;
};

#endif /* HTTPPARSER_HPP */
