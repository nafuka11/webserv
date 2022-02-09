#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"

class CGIParser : public HTTPParser
{
public:
    CGIParser(HTTPRequest &request, const std::vector<ServerConfig> &configs, HTTPResponse &response);
    ~CGIParser();
    void clear();
    void parse();
private:
    static const std::string NEWLINE;

    HTTPResponse &response_;
    std::map<std::string, std::string> raw_cgi_headers_;

    bool parseHeader();
    bool parseMessageBody();
    void splitHeader(const std::string &line,
                     std::string &header_name, std::string &header_value);
    bool isValidHeaders();
    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    void validateStatus(const std::string &value);
    void generateResponseHeaders();
};

#endif /* CGIPARSER_HPP */
