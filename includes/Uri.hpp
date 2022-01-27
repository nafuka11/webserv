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

    Uri(const ServerConfig &server_config,
        const std::string &uri, const std::string &method);
    ~Uri();

    const LocationConfig *getLocationConfig() const;
    const std::string &getRawPath() const;
    const std::string &getLocalPath() const;
    Type getResourceType() const;
    const struct stat &getStat() const;

    bool isRegularFile(const struct stat &path_stat) const;
    bool isDirectory(const struct stat &path_stat) const;
    bool canWrite(const struct stat &path_stat) const;

private:
    const ServerConfig &server_config_;
    const LocationConfig *location_config_;
    const std::string &method_;

    const std::string &raw_uri_;
    std::string raw_path_;
    std::string local_path_;
    std::string query_;
    Type resource_type_;
    struct stat stat_;

    void splitRawUri();
    void findPath();
    void findPathFromLocation(const std::string &location_name,
                              const LocationConfig &location, std::string &path);
    void findFileFromIndexes(const LocationConfig &location, std::string &path);

    bool startsWith(const std::string &str, const std::string &prefix) const;
    bool needAutoIndex(const LocationConfig &config, const std::string &path) const;
    bool execStat(const std::string &path, struct stat *buf) const;
};

#endif /* URI_HPP */
