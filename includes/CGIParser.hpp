#ifndef CGIPARSER_HPP
#define CGIPARSER_HPP

#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"

class CGIParser : public HTTPParser
{
public:
    CGIParser(HTTPRequest &request, const ServerConfig &config);
    ~CGIParser();
    void parse(HTTPResponse &respons);
private:
    static const std::string NEWLINE;

    const std::pair<std::string, std::string> validateHeader(std::string &name,
                                                             std::string &value);
    void validateStatus();
};

#endif /* CGIPARSER_HPP */
