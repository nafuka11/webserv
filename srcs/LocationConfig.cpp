#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
: alias_(),
  allow_method_(),
  autoindex_(),
  error_page_(),
  index_(),
  return_redirect_(),
  upload_path_()
{
}

LocationConfig::~LocationConfig()
{
}

void LocationConfig::setAlias(const std::string &path)
{
    alias_ = path;
}

void LocationConfig::setAutoindex(const std::string &autoindex)
{
    autoindex_ = autoindex;
}

void LocationConfig::setUploadPath(const std::string &path)
{
    upload_path_ = path;
}

void LocationConfig::addAllowMethod(const std::string &method)
{
    allow_method_.push_back(method);
}

void LocationConfig::addErrorPage(const int status_code, const std::string &uri)
{
    error_page_.insert(std::make_pair(status_code, uri));
}

void LocationConfig::addIndex(const std::string &file)
{
    index_.push_back(file);
}

void LocationConfig::addReturnRedirect(const int status_code, const std::string &uri)
{
    return_redirect_.insert(std::make_pair(status_code, uri));
}

void LocationConfig::clearAllowMethod()
{
    allow_method_.clear();
}
void LocationConfig::clearErrorPage(const int status_code)
{
    std::map<int, std::string>::iterator iter = error_page_.find(status_code);

    if (iter != error_page_.end())
    {
        error_page_.erase(iter);
    }
}
void LocationConfig::clearIndex()
{
    index_.clear();
}
void LocationConfig::clearReturnRedirect(const int status_code)
{
    std::map<int, std::string>::iterator iter = return_redirect_.find(status_code);

    if (iter != return_redirect_.end())
    {
        return_redirect_.erase(iter);
    }
}

const std::string LocationConfig::alias() const
{
    return alias_;
}

const std::vector<std::string> LocationConfig::allowMethod() const
{
    return allow_method_;
}

const std::string LocationConfig::autoindex() const
{
    return autoindex_;
}

const std::map<int, std::string> LocationConfig::errorPage() const
{
    return error_page_;
}

const std::vector<std::string> LocationConfig::index() const
{
    return index_;
}

const std::map<int, std::string> LocationConfig::returnRedirect() const
{
    return return_redirect_;
}

const std::string LocationConfig::uploadPath() const
{
    return upload_path_;
}
