#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HTTPRequest
{
public:
    static const std::string HTTP_GET;
    static const std::string HTTP_POST;
    static const std::string HTTP_DELETE;

    HTTPRequest();
    ~HTTPRequest();
    void clear();
    bool canKeepAlive();

    void setMethod(const std::string &method);
    void setUri(const std::string &uri);
    void setProtocolVersion(const std::string &protocol_version);
    void setHeader(const std::pair<std::string, std::string> &item);
    void setMessageBody(const std::string &message_body);

    const std::string &getMethod() const;
    const std::string &getUri() const;
    const std::string &getProtocolVersion() const;
    const std::map<std::string, std::string> getHeaders() const;
    const std::string &getMessageBody() const;

private:
    std::string method_;
    std::string uri_;
    std::string protocol_version_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;
};

#endif /* HTTPREQUEST_HPP */
