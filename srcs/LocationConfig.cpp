#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
{
}

LocationConfig::~LocationConfig()
{
}

void LocationConfig::setAlias(const std::string path)
{
    alias_ = path;
}

void LocationConfig::setAutoIndex(const std::string autoindex)
{
    autoindex_ = autoindex;
}
void LocationConfig::addIndex(const std::string file)
{
    index_.push_back(file);
}

void LocationConfig::addReturnRedirect(const int status_code, const std::string uri)
{
    return_.insert(std::make_pair(status_code, uri));
}

const std::string LocationConfig::alias() const
{
    return alias_;
}

const std::string LocationConfig::autoindex() const
{
    return autoindex_;
}

const std::vector<std::string> LocationConfig::index() const
{
    return index_;
}

const std::map<int, std::string> LocationConfig::returnRedirect() const
{
    return return_;
}
