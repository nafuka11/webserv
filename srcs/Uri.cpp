#include "Uri.hpp"
#include <algorithm>
#include <cerrno>
#include "HTTPParseException.hpp"

Uri::Uri(const ServerConfig &config, const std::string &uri)
    : config_(config), raw_uri_(uri), resource_type_(INVALID)
{
    splitRawUri();
    findPath();
}

Uri::~Uri()
{
}

std::string Uri::getRawUri() const
{
    return raw_uri_;
}

std::string Uri::getPath() const
{
    return path_;
}

Uri::Type Uri::getResourceType() const
{
    return resource_type_;
}

void Uri::splitRawUri()
{
    size_t query_pos = raw_uri_.find('?');
    path_ = raw_uri_.substr(0, query_pos);
    query_ = raw_uri_.substr(query_pos + 1);
}

void Uri::findPath()
{
    std::string path = path_;
    const std::map<std::string, LocationConfig> &locations = config_.location();
    std::map<std::string, LocationConfig>::const_reverse_iterator iter = locations.rbegin();

    for (; iter != locations.rend(); ++iter)
    {
        if (!startsWith(path_, iter->first))
        {
            continue;
        }
        findPathFromLocation(iter->first, iter->second, path);
        path_ = path;
        break;
    }
}

void Uri::findPathFromLocation(const std::string &location_name,
                               const LocationConfig &location, std::string &path)
{
    if (!location.alias().empty())
    {
        path = path.replace(0, location_name.size(), location.alias());
    }
    if (path.at(path.size() - 1) == '/')
    {
        findFileFromIndexes(location, path);
        return;
    }

    struct stat path_stat;
    if (!execStat(path, &path_stat))
    {
        throw HTTPParseException(CODE_404);
    }
    if (isRegularFile(path_stat))
    {
        resource_type_ = FILE;
        return;
    }
    if (!needAutoIndex(location, path))
    {
        throw HTTPParseException(CODE_404);
    }
    resource_type_ = AUTOINDEX;
}

void Uri::findFileFromIndexes(const LocationConfig &location, std::string &path)
{
    struct stat path_stat;
    for (size_t i = 0; i < location.index().size(); i++)
    {
        std::string joined_path = path + location.index()[i];
        if (!execStat(joined_path, &path_stat))
        {
            continue;
        }
        if (isRegularFile(path_stat))
        {
            path = joined_path;
            resource_type_ = FILE;
            return;
        }
    }

    if (!needAutoIndex(location, path))
    {
        throw HTTPParseException(CODE_404);
    }
    resource_type_ = AUTOINDEX;
}

bool Uri::startsWith(const std::string &str, const std::string &prefix) const
{
    return str.size() >= prefix.size() &&
           std::equal(std::begin(prefix), std::end(prefix), std::begin(str));
}

bool Uri::needAutoIndex(const LocationConfig &config, const std::string &path) const
{
    if (config.autoindex() != "on")
    {
        return false;
    }

    struct stat path_stat;
    if (!execStat(path, &path_stat))
    {
        return false;
    }
    return isDirectory(path_stat);
}

bool Uri::execStat(const std::string &path, struct stat *path_stat) const
{
    int result = stat(path.c_str(), path_stat);
    if (result < 0)
    {
        return false;
    }
    return true;
}

bool Uri::isRegularFile(const struct stat &path_stat) const
{
    return S_ISREG(path_stat.st_mode);
}

bool Uri::isDirectory(const struct stat &path_stat) const
{
    return S_ISDIR(path_stat.st_mode);
}
