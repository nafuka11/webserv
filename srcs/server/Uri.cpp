#include "Uri.hpp"
#include <algorithm>
#include <cerrno>
#include "HTTPParseException.hpp"
#include "HTTPRequest.hpp"

Uri::Uri(const ServerConfig &server_config,
         const std::string &uri, const std::string &method)
    : server_config_(server_config), location_config_(NULL), method_(method),
      raw_uri_(uri), resource_type_(INVALID)
{
    splitRawUri();
    findPath();
}

Uri::~Uri()
{
}

const LocationConfig *Uri::getLocationConfig() const
{
    return location_config_;
}

const std::string &Uri::getQuery() const
{
    return query_;
}

const std::vector<std::string> &Uri::getArguments() const
{
    return arguments_;
}


const std::string &Uri::getRawPath() const
{
    return raw_path_;
}

const std::string &Uri::getLocalPath() const
{
    return local_path_;
}

Uri::Type Uri::getResourceType() const
{
    return resource_type_;
}

const struct stat &Uri::getStat() const
{
    return stat_;
}

bool Uri::canWrite(const struct stat &path_stat) const
{
    return (path_stat.st_mode & S_IWUSR) == S_IWUSR;
}

void Uri::splitRawUri()
{
    size_t query_pos = raw_uri_.find('?');
    if (query_pos == std::string::npos)
    {
        raw_path_ = raw_uri_;
        query_ = "";
        return ;
    }
    raw_path_ = raw_uri_.substr(0, query_pos);
    query_ = raw_uri_.substr(query_pos + 1);
    parseQuery();
}

void Uri::parseQuery()
{
    size_t found = query_.find('=');
    if (found != std::string::npos)
        return;

    std::string split_word = "+";
    size_t start_pos = 0;
    for (size_t split_pos = query_.find(split_word);
         split_pos != std::string::npos;
         split_pos = query_.find(split_word, start_pos))
    {
        arguments_.push_back(query_.substr(start_pos, (split_pos - start_pos)));
        start_pos = split_pos + split_word.size();
    }
    arguments_.push_back(query_.substr(start_pos));
}


void Uri::findPath()
{
    std::string path = raw_path_;
    const std::map<std::string, LocationConfig> &locations = server_config_.location();
    std::map<std::string, LocationConfig>::const_reverse_iterator
        iter = locations.rbegin();

    for (; iter != locations.rend(); ++iter)
    {
        if (!startsWith(raw_path_, iter->first))
        {
            continue;
        }
        location_config_ = &iter->second;
        findPathFromLocation(iter->first, iter->second, path);
        local_path_ = path;
        break;
    }
}

void Uri::findPathFromLocation(const std::string &location_name,
                               const LocationConfig &location, std::string &path)
{
    if (location.returnRedirect().size())
    {
        resource_type_ = REDIRECT;
        return;
    }
    if (!location.alias().empty())
    {
        path = path.replace(0, location_name.size(), location.alias());
    }
    if (path.at(path.size() - 1) == '/' && method_ == HTTPRequest::HTTP_GET)
    {
        findFileFromIndexes(location, path);
        return;
    }
    if (method_ == HTTPRequest::HTTP_POST && !location.uploadPath().empty())
    {
        if (raw_path_ == location_name)
        {
            resource_type_ = FILE;
            return;
        }
        else
        {
            throw HTTPParseException(CODE_403);
        }
    }

    struct stat path_stat;
    if (!execStat(path, &path_stat))
    {
        throw HTTPParseException(CODE_404);
    }
    if (isDirectory(path_stat) && path.at(path.size() - 1) != '/')
    {
        resource_type_ = REDIRECT;
        return;
    }
    if (isRegularFile(path_stat))
    {
        if (hasCgiExtension(path, location.cgiExtension()))
        {
            resource_type_ = CGI;
        }
        else
        {
            resource_type_ = FILE;
        }
        stat_ = path_stat;
        return;
    }
    if (!needAutoIndex(location, path))
    {
        if (method_ == HTTPRequest::HTTP_GET)
        {
            throw HTTPParseException(CODE_404);
        }
        else
        {
            resource_type_ = FILE;
            stat_ = path_stat;
            return;
        }
    }
    resource_type_ = AUTOINDEX;
    stat_ = path_stat;
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
            if (hasCgiExtension(path, location.cgiExtension()))
            {
                resource_type_ = CGI;
            }
            else
            {
                resource_type_ = FILE;
            }
            stat_ = path_stat;
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
    if (method_ != HTTPRequest::HTTP_GET)
    {
        return false;
    }
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

bool Uri::hasCgiExtension(const std::string &path,
                          const std::vector<std::string> &cgi_extension) const
{
    if (cgi_extension.empty())
    {
        return false;
    }

    size_t extension_pos = path.rfind(".");
    if (extension_pos == std::string::npos)
    {
        return false;
    }

    std::string extension = path.substr(extension_pos);
    std::vector<std::string>::const_iterator found =
        std::find(cgi_extension.begin(), cgi_extension.end(), extension);
    if (found == cgi_extension.end())
    {
        return false;
    }
    return true;
}
