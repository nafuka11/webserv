#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <map>
#include <string>
#include <ctime>
#include <dirent.h>
#include "HTTPStatusCode.hpp"
#include "Uri.hpp"

class HTTPResponse
{
public:
    HTTPResponse();
    ~HTTPResponse();
    std::string toString(const LocationConfig *location);
    void appendMessageBody(const char *body);
    void clear();
    void setStatusCode(HTTPStatusCode status_code);
    void setKeepAlive(bool keep_alive);
    std::string generateAutoindexHTML(const Uri &uri, DIR *dir_p) const;

private:
    static const std::string CRLF;
    static const std::map<HTTPStatusCode, std::string> REASON_PHRASE;
    static const size_t DATE_STR_LEN;
    static const size_t MTIME_STR_LEN;
    static const size_t AUTOINDEX_FILENAME_WIDTH;
    static const size_t AUTOINDEX_MTIME_WIDTH;
    static const size_t AUTOINDEX_FILESIZE_WIDTH;

    HTTPStatusCode status_code_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;
    bool keep_alive_;

    static std::map<HTTPStatusCode, std::string> setReasonPhrase();
    void setProperties(const LocationConfig *location);
    std::string generateHTMLfromStatusCode() const;
    std::string generateDateString() const;
    void appendFileHTML(std::stringstream &ss,
                        const Uri &uri, const struct dirent *entry) const;
    std::string generateHref(const struct dirent *entry) const;
    std::string generateFilename(const struct dirent *entry) const;
    std::string generateMtime(const struct stat *path_stat) const;
    std::string escapeHTML(const std::string &str) const;
    std::string escapeUri(const std::string &str) const;
};

#endif /* HTTPRESPONSE_HPP */
