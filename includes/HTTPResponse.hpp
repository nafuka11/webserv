#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>
#include <ctime>
#include "HTTPStatusCode.hpp"

class HTTPResponse
{
public:
    HTTPResponse(HTTPStatusCode status_code);
    ~HTTPResponse();
    std::string toString();

private:
    static const std::map<HTTPStatusCode, std::string> REASON_PHRASE;
    static const size_t DATE_STR_LEN;

    HTTPStatusCode status_code_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;

    static std::map<HTTPStatusCode, std::string> setReasonPhrase();
    bool canKeepAlive() const;
    std::string generateHTMLfromStatusCode() const;
    std::string generateDateString() const;
};

#endif /* HTTPRESPONSE_HPP */
