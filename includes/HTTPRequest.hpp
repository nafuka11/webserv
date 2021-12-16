#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include "HTTPMethod.hpp"

class HTTPRequest
{
public:
    HTTPRequest();
    ~HTTPRequest();
    void clear();
    bool canKeepAlive();

    void setMethod(HTTPMethod method);
    void setUri(const std::string &uri);
    void setProtocolVersion(const std::string &protocol_version);
    void setHeader(const std::pair<std::string, std::string> &item);
    void setMessageBody(const std::string &message_body);

    HTTPMethod getMethod() const;
    const std::string &getUri() const;
    const std::string &getProtocolVersion() const;
    const std::map<std::string, std::string> getHeaders() const;
    const std::string &getMessageBody() const;

private:
    HTTPMethod method_;
    std::string uri_;
    std::string protocol_version_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;
};

#endif /* HTTPREQUEST_HPP */
