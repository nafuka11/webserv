#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest()
{
}

HTTPRequest::~HTTPRequest()
{
}

void HTTPRequest::clear()
{
    uri_.clear();
    protocol_version_.clear();
    headers_.clear();
    message_body_.clear();
}

bool HTTPRequest::canKeepAlive()
{
    std::map<std::string, std::string>::const_iterator item = headers_.find("connection");

    if (item != headers_.end() && item->second == "close")
    {
        return false;
    }
    return true;
}

void HTTPRequest::setMethod(HTTPMethod method)
{
    method_ = method;
}

void HTTPRequest::setUri(const std::string &uri)
{
    uri_ = uri;
}

void HTTPRequest::setProtocolVersion(const std::string &protocol_version)
{
    protocol_version_ = protocol_version;
}

void HTTPRequest::setHeader(const std::string &name, const std::string &value)
{
    headers_.insert(std::make_pair(name, value));
}

void HTTPRequest::setMessageBody(const std::string &message_body)
{
    message_body_ = message_body;
}

HTTPMethod HTTPRequest::getMethod() const
{
    return method_;
}

const std::string &HTTPRequest::getUri() const
{
    return uri_;
}

const std::string &HTTPRequest::getProtocolVersion() const
{
    return protocol_version_;
}

const std::map<std::string, std::string> HTTPRequest::getHeaders() const
{
    return headers_;
}

const std::string &HTTPRequest::getMessageBody() const
{
    return message_body_;
}
