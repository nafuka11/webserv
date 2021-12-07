#include "ServerConfig.hpp"
#include <utility>

ServerConfig::ServerConfig()
{
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::setListen(const int listen)
{
    listen_ = listen;
}

void ServerConfig::setServerName(const std::string name)
{
    server_name_ = name;
}

void ServerConfig::addAllowMethods(const std::string method)
{
    allow_methods_.push_back(method);
}

void ServerConfig::setcgiExtension(const std::string extension)
{
    cgi_extension_ = extension;
}

void ServerConfig::setClientMaxBodySize(const int size)
{
    client_max_body_size_ = size;
}

void ServerConfig::addErrorPage(const int code, const std::string uri)
{
    error_page_.insert(std::map<int, std::string>::value_type(code, uri));
}

void ServerConfig::setUploadPath(const std::string path)
{
    upload_path_ = path;
}

// void ServerConfig::addLocation(const std::string path, const LocationConfig location_config)
// {
//     location_.insert(std::map<std::string, LocationConfig>::value_type(path, location_config));
// }

const int ServerConfig::listen() const
{
    return listen_;
}

const std::string ServerConfig::serverName() const
{
    return server_name_;
}

const std::vector<std::string> ServerConfig::allowMethods() const
{
    return allow_methods_;
}

const std::string ServerConfig::cgiExtension() const
{
    return cgi_extension_;
}

const int ServerConfig::clientMaxBodySize() const
{
    return client_max_body_size_;
}

const std::map<int, std::string> ServerConfig::errorPage() const
{
    return error_page_;
}

const std::string ServerConfig::uploadPath() const
{
    return upload_path_;
}

// const std::map<std::string, LocationConfig> ServerConfig::location()
// {
//     return location_
// }