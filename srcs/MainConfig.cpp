#include "MainConfig.hpp"
#include "ServerConfig.hpp"

const int MainConfig::DEFAULT_CLIENT_MAX_BODY_SIZE = 5000;

MainConfig::MainConfig()
: allow_method_(),
  autoindex_(),
  client_max_body_size_(DEFAULT_CLIENT_MAX_BODY_SIZE),
  cgi_extension_(),
  error_page_(),
  index_(),
  server_()
{
}

MainConfig::~MainConfig()
{
}

void MainConfig::addAllowMethod(const std::string &method)
{
    allow_method_.push_back(method);
}

void MainConfig::setAutoIndex(const std::string &autoindex)
{
    autoindex_ = autoindex;
}

void MainConfig::setClientMaxBodySize(const int size)
{
    client_max_body_size_ = size;
}

void MainConfig::setcgiExtension(const std::string &extension)
{
    cgi_extension_ = extension;
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

const std::vector<std::string> MainConfig::allowMethods() const
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
    return cgi_extension_;
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
