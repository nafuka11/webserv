#include "MainConfig.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

std::string default_allow_method[] = {"GET", "POST", "DELETE"};
const std::vector<std::string> MainConfig::DEFAULT_ALLOW_METHOD(default_allow_method, std::end(default_allow_method));
const std::string MainConfig::DEFAULT_AUTOINDEX = "off";
const int MainConfig::DEFAULT_CLIENT_MAX_BODY_SIZE = 5000;
std::string default_index[] = {"index.html"};
const std::vector<std::string> MainConfig::DEFAULT_INDEX(default_index, std::end(default_index));

MainConfig::MainConfig()
: allow_method_(DEFAULT_ALLOW_METHOD),
  autoindex_(DEFAULT_AUTOINDEX),
  client_max_body_size_(DEFAULT_CLIENT_MAX_BODY_SIZE),
  cgi_extension_(),
  error_page_(),
  index_(DEFAULT_INDEX),
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

void MainConfig::clearAllowMethod()
{
    allow_method_.clear();
}

void MainConfig::clearErrorPage(int status_code)
{
    // error_page_.clear();
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
