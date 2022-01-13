#include "MainConfig.hpp"
#include "ConfigConstant.hpp"

MainConfig::MainConfig()
: allow_method_(),
  autoindex_(),
  cgi_extensions_(),
  client_max_body_size_(ConfigConstant::UNSET_TYPE_INT),
  error_page_(),
  index_()
{
}

MainConfig::~MainConfig()
{
}

void MainConfig::setAutoindex(const std::string &autoindex)
{
    autoindex_ = autoindex;
}

void MainConfig::setClientMaxBodySize(const int size)
{
    client_max_body_size_ = size;
}

void MainConfig::setCgiExtension(const std::string &extension)
{
    cgi_extensions_ = extension;
}

void MainConfig::addAllowMethod(const std::string &method)
{
    allow_method_.push_back(method);
}

void MainConfig::addErrorPage(const int status_code, const std::string &uri)
{
    error_page_.insert(std::make_pair(status_code, uri));
}

void MainConfig::addIndex(const std::string &file)
{
    index_.push_back(file);
}

void MainConfig::addServer(const ServerConfig &server_config)
{
    server_.push_back(server_config);
}

void MainConfig::clearAllowMethod()
{
    allow_method_.clear();
}

void MainConfig::clearErrorPage(int status_code)
{
    std::map<int, std::string>::iterator iter = error_page_.find(status_code);

    if (iter != error_page_.end())
    {
        error_page_.erase(iter);
    }
}

void MainConfig::clearIndex()
{
    index_.clear();
}

const std::vector<std::string> MainConfig::allowMethod() const
{
    return allow_method_;
}

const std::string MainConfig::autoindex() const
{
    return autoindex_;
}

const int &MainConfig::clientMaxBodySize() const
{
    return client_max_body_size_;
}

const std::string MainConfig::cgiExtension() const
{
    return cgi_extensions_;
}

const std::map<int, std::string> MainConfig::errorPage() const
{
    return error_page_;
}

const std::vector<std::string> MainConfig::index() const
{
    return index_;
}

const std::vector<ServerConfig> &MainConfig::server() const
{
    return server_;
}
