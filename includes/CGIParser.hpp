#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"

class CGIParser : public HTTPParser
{
public:
    CGIParser(HTTPRequest &request, const ServerConfig &config);
    ~CGIParser();
    void parse(HTTPResponse &response);
private:
    static const std::string NEWLINE;
    void splitHeader(const std::string &line,
                     std::string &header_name, std::string &header_value);
    bool isValidHeaders(const std::map<std::string, std::string> &headers);
    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    void validateStatus();
};

#endif /* CGIPARSER_HPP */
