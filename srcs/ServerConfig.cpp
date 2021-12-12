#include "ServerConfig.hpp"
#include <utility>
#include "MainConfig.hpp"

const int ServerConfig::DEFAULT_PORT = 80;

ServerConfig::ServerConfig(const MainConfig &main_config)
: allow_method_(main_config.allowMethod()),
  autoindex_(main_config.autoindex()),
  client_max_body_size_(main_config.clientMaxBodySize()),
  error_page_(main_config.errorPage()),
  index_(main_config.index()),
  listen_(DEFAULT_PORT),
  location_(),
  return_(),
  server_name_(),
  upload_path_()
{
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::addAllowMethods(const std::string &method)
{
    allow_method_.push_back(method);
}

void ServerConfig::setAutoIndex(const std::string &autoindex)
{
    autoindex_ = autoindex;
}

void ServerConfig::setClientMaxBodySize(const int size)
{
    client_max_body_size_ = size;
}

void ServerConfig::addErrorPage(const int status_code, const std::string &uri)
{
    error_page_.insert(std::make_pair(status_code, uri));
}

void ServerConfig::addIndex(const std::string &file)
{
    index_.push_back(file);
}

void ServerConfig::setListen(const int port)
{
    listen_ = port;
}

void ServerConfig::addLocation(const std::string &path, const LocationConfig &location_config)
{
    location_.insert(std::make_pair(path, location_config));
}

void ServerConfig::addReturnRedirect(const int status_code, const std::string &uri)
{
    return_.insert(std::make_pair(status_code, uri));
}

void ServerConfig::setServerName(const std::string &name)
{
    server_name_ = name;
}

const std::vector<std::string> ServerConfig::allowMethod() const
{
    return allow_method_;
}

const std::string ServerConfig::autoindex() const
{
    return autoindex_;
}

const int &ServerConfig::clientMaxBodySize() const
{
    return client_max_body_size_;
}

const std::map<int, std::string> ServerConfig::errorPage() const
{
    return error_page_;
}

const std::vector<std::string> ServerConfig::index() const
{
    return index_;
}

const int &ServerConfig::listen() const
{
    return listen_;
}

const std::map<std::string, LocationConfig> ServerConfig::location() const
{
    return location_;
}

const std::map<int, std::string> ServerConfig::returnRedirect() const
{
    return return_;
}

const std::string ServerConfig::serverName() const
{
    return server_name_;
}

const std::string ServerConfig::uploadPath() const
{
    return upload_path_;
}
