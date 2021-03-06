#include "HTTPRequest.hpp"

const std::string HTTPRequest::HTTP_GET = "GET";
const std::string HTTPRequest::HTTP_POST = "POST";
const std::string HTTPRequest::HTTP_DELETE = "DELETE";

HTTPRequest::HTTPRequest() : server_config_(NULL)
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

void HTTPRequest::appendMessageBody(const std::string &message_body)
{
    message_body_.append(message_body);
}

void HTTPRequest::setMethod(const std::string &method)
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

void HTTPRequest::setHeader(const std::pair<std::string, std::string> &item)
{
    headers_.insert(item);
}

void HTTPRequest::setMessageBody(const std::string &message_body)
{
    message_body_ = message_body;
}

void HTTPRequest::setServerConfig(const ServerConfig *server_config)
{
    server_config_ = server_config;
}

void HTTPRequest::setLocation(const std::string &location)
{
    location_ = location;
}

const std::string &HTTPRequest::getMethod() const
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

const std::map<std::string, std::string> &HTTPRequest::getHeaders() const
{
    return headers_;
}

const std::string &HTTPRequest::getMessageBody() const
{
    return message_body_;
}

const ServerConfig *HTTPRequest::getServerConfig() const
{
    return server_config_;
}

const std::string &HTTPRequest::getLocation() const
{
    return location_;
}
