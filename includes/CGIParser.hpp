#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"

class CGIParser : public HTTPParser
{
public:
    CGIParser(HTTPRequest &request, const ServerConfig &config, HTTPResponse &response);
    ~CGIParser();
    void parse();
private:
    static const std::string NEWLINE;

    HTTPResponse &response_;
    std::map<std::string, std::string> headers_;

    void splitHeader(const std::string &line,
                     std::string &header_name, std::string &header_value);
    bool isValidHeaders();
    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    void validateStatus(const std::string &value);
    void setInResponseHeaders();
};

#endif /* CGIPARSER_HPP */
