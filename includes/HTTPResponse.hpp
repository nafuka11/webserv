#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>
#include <ctime>
#include "HTTPStatusCode.hpp"

class HTTPResponse
{
public:
    HTTPResponse();
    ~HTTPResponse();
    std::string toString();
    void appendMessageBody(const char *body);
    void clear();
    void setStatusCode(HTTPStatusCode status_code);
    void setKeepAlive(bool keep_alive);

private:
    static const std::map<HTTPStatusCode, std::string> REASON_PHRASE;
    static const size_t DATE_STR_LEN;

    HTTPStatusCode status_code_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;
    bool keep_alive_;

    static std::map<HTTPStatusCode, std::string> setReasonPhrase();
    void setProperties();
    std::string generateHTMLfromStatusCode() const;
    std::string generateDateString() const;
};

#endif /* HTTPRESPONSE_HPP */
