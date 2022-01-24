#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <sys/stat.h>
#include "ServerConfig.hpp"

class Uri
{
public:
    enum Type {
        FILE,
        AUTOINDEX,
        REDIRECT,
        CGI,
        INVALID
    };

    Uri(const ServerConfig &config, const std::string &uri);
    ~Uri();

    std::string getRawUri() const;
    std::string getPath() const;
    Type getResourceType() const;

private:
    const ServerConfig &config_;
    const std::string &raw_uri_;
    std::string query_;
    std::string path_;
    Type resource_type_;

    void splitRawUri();
    void findPath();
    void findPathFromLocation(const std::string &location_name,
                              const LocationConfig &location, std::string &path);
    void findFileFromIndexes(const LocationConfig &location, std::string &path);

    bool startsWith(const std::string &str, const std::string &prefix) const;
    bool needAutoIndex(const LocationConfig &config, const std::string &path) const;
    bool execStat(const std::string &path, struct stat *buf) const;
    bool isRegularFile(const struct stat &path_stat) const;
    bool isDirectory(const struct stat &path_stat) const;
    bool isExecutable(const struct stat &path_stat) const;
    bool hasCgiExtension(const std::string &path,
                         const std::vector<std::string> &cgi_extension) const;
};

#endif /* URI_HPP */
