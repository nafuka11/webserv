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
    std::string CGItoString(const LocationConfig *location);
    void appendMessageBody(const char *body, size_t size);
    void clear();
    void setRedirectResponse(int status_code, const std::string &uri);
    std::string generateAutoindexHTML(const Uri &uri, DIR *dir_p) const;
    std::string generateHTMLfromStatusCode(HTTPStatusCode statusCode) const;

    void setHeader(const std::pair<std::string, std::string> &item);
    void setStatusCode(int status_code);
    void setKeepAlive(bool keep_alive);
    void setMessageBody(const std::string &body);
    void setSentByte(size_t sent_byte);

    const std::map<std::string, std::string> getHeaders() const;
    const std::string &getRawMessage() const;
    size_t getSentByte() const;

private:
    static const std::string CRLF;
    static const std::map<int, std::string> REASON_PHRASE;
    static const size_t DATE_STR_LEN;
    static const size_t MTIME_STR_LEN;
    static const size_t AUTOINDEX_FILENAME_WIDTH;
    static const size_t AUTOINDEX_MTIME_WIDTH;
    static const size_t AUTOINDEX_FILESIZE_WIDTH;

    int status_code_;
    std::map<std::string, std::string> headers_;
    std::string message_body_;
    bool keep_alive_;
    std::string raw_message_;
    size_t sent_byte_;

    static std::map<int, std::string> setReasonPhrase();
    void setProperties(const LocationConfig *location);
    std::string findReasonPhrase(int status_code) const;
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
