#include "ServerConfig.hpp"
#include <utility>
#include "ConfigConstant.hpp"

ServerConfig::ServerConfig()
: allow_method_(),
  autoindex_(),
  cgi_extension_(),
  client_max_body_size_(ConfigConstant::UNSET_TYPE_INT),
  error_page_(),
  index_(),
  listen_(ConfigConstant::UNSET_TYPE_INT),
  return_redirect_(),
  server_name_(),
  upload_path_()
{
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::setAutoindex(const std::string &autoindex)
{
    autoindex_ = autoindex;
}

void ServerConfig::setClientMaxBodySize(const int size)
{
    client_max_body_size_ = size;
}

void ServerConfig::setListen(const int port)
{
    listen_ = port;
}

void ServerConfig::setServerName(const std::string &name)
{
    server_name_ = name;
}

void ServerConfig::setUploadPath(const std::string &path)
{
    upload_path_ = path;
}

void ServerConfig::addAllowMethod(const std::string &method)
{
    allow_method_.push_back(method);
}

void ServerConfig::addCgiExtension(const std::string &extension)
{
    cgi_extension_.push_back(extension);
}

void ServerConfig::addErrorPage(const int status_code, const std::string &uri)
{
    error_page_.insert(std::make_pair(status_code, uri));
}

void ServerConfig::addIndex(const std::string &file)
{
    index_.push_back(file);
}

void ServerConfig::addLocation(const std::string &path, const LocationConfig &location_config)
{
    location_.insert(std::make_pair(path, location_config));
}

void ServerConfig::addReturnRedirect(const int status_code, const std::string &uri)
{
    return_redirect_.insert(std::make_pair(status_code, uri));
}

const std::vector<std::string> &ServerConfig::allowMethod() const
{
    return allow_method_;
}

const std::string ServerConfig::autoindex() const
{
    return autoindex_;
}

const std::vector<std::string> &ServerConfig::cgiExtension() const
{
    return cgi_extension_;
}

const int &ServerConfig::clientMaxBodySize() const
{
    return client_max_body_size_;
}

const std::map<int, std::string> &ServerConfig::errorPage() const
{
    return error_page_;
}

const std::vector<std::string> &ServerConfig::index() const
{
    return index_;
}

const int &ServerConfig::listen() const
{
    return listen_;
}

const std::map<std::string, LocationConfig> &ServerConfig::location() const
{
    return location_;
}

const std::map<int, std::string> &ServerConfig::returnRedirect() const
{
    return return_redirect_;
}

const std::string ServerConfig::serverName() const
{
    return server_name_;
}

const std::string ServerConfig::uploadPath() const
{
    return upload_path_;
}
