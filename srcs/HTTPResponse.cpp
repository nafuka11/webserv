#include "HTTPResponse.hpp"
#include <sstream>
#include <iomanip>

const std::string HTTPResponse::CRLF = "\r\n";
const std::map<HTTPStatusCode, std::string> HTTPResponse::REASON_PHRASE = HTTPResponse::setReasonPhrase();
const size_t HTTPResponse::DATE_STR_LEN = 40;
const size_t HTTPResponse::MTIME_STR_LEN = 20;
const size_t HTTPResponse::AUTOINDEX_FILENAME_WIDTH = 50;
const size_t HTTPResponse::AUTOINDEX_MTIME_WIDTH = 17;
const size_t HTTPResponse::AUTOINDEX_FILESIZE_WIDTH = 20;

HTTPResponse::HTTPResponse()
{
}

HTTPResponse::~HTTPResponse()
{
}

std::string HTTPResponse::toString(const LocationConfig *location)
{
    setProperties(location);

    std::stringstream ss;

    std::string phrase = HTTPResponse::REASON_PHRASE.find(status_code_)->second;
    ss << "HTTP/1.1 " << status_code_ << " " << phrase << "\r\n";
    for (std::map<std::string, std::string>::iterator iter = headers_.begin();
         iter != headers_.end();
         ++iter)
    {
        ss << iter->first << ": " << iter->second << "\r\n";
    }
    ss << "\r\n";
    ss << message_body_;
    return ss.str();
}

void HTTPResponse::appendMessageBody(const char *body)
{
    message_body_.append(body);
}

void HTTPResponse::clear()
{
    message_body_.clear();
}

void HTTPResponse::setStatusCode(HTTPStatusCode status_code)
{
    status_code_ = status_code;
}

void HTTPResponse::setKeepAlive(bool keep_alive)
{
    keep_alive_ = keep_alive;
}

void HTTPResponse::setMessageBody(const std::string &body)
{
    message_body_ = body;
}

std::map<HTTPStatusCode, std::string> HTTPResponse::setReasonPhrase()
{
    std::map<HTTPStatusCode, std::string> reason_phrase;

    reason_phrase[CODE_200] = "OK";
    reason_phrase[CODE_400] = "Bad Request";
    reason_phrase[CODE_404] = "Not Found";
    reason_phrase[CODE_405] = "Method Not Allowed";
    reason_phrase[CODE_413] = "Payload Too Large";
    reason_phrase[CODE_501] = "Not Implemented";
    return reason_phrase;
}

void HTTPResponse::setProperties(const LocationConfig *location)
{
    if (status_code_ != CODE_200)
    {
        message_body_ = generateHTMLfromStatusCode(status_code_);
    }
    std::stringstream ss;
    ss << message_body_.size();
    std::string content_length = ss.str();

    headers_.clear();
    headers_.insert(std::make_pair("Server", "webserv/1.0.0"));
    headers_.insert(std::make_pair("Date", generateDateString()));
    headers_.insert(std::make_pair("Content-Length", content_length));
    if (keep_alive_)
    {
        headers_.insert(std::make_pair("Connection", "keep-alive"));
    }
    else
    {
        headers_.insert(std::make_pair("Connection", "close"));
    }
    if (status_code_ == CODE_405)
    {
        std::string allow_value;
        const std::vector<std::string> &allow_methods = location->allowMethod();
        for (size_t i = 0; i < allow_methods.size(); i++)
        {
            allow_value.append(allow_methods.at(i));
            if (i < allow_methods.size() - 1)
            {
                allow_value.append(", ");
            }
        }
        headers_.insert(std::make_pair("Allow", allow_value));
    }
}

std::string HTTPResponse::generateHTMLfromStatusCode(HTTPStatusCode statusCode) const
{
    std::stringstream ss;

    std::string phrase = HTTPResponse::REASON_PHRASE.find(statusCode)->second;
    ss << "<html>"
       << "\r\n"
       << "<head><title>" << statusCode << " " << phrase << "</title></head>"
       << "\r\n"
       << "<body>"
       << "\r\n"
       << "<center><h1>" << statusCode << " " << phrase << "</h1></center>"
       << "<hr><center>webserv/1.0.0</center>"
       << "\r\n"
       << "</body>"
       << "\r\n"
       << "</html>"
       << "\r\n";
    return ss.str();
}

std::string HTTPResponse::generateDateString() const
{
    time_t now = time(NULL);
    struct tm *now_time = gmtime(&now);
    char str[DATE_STR_LEN];

    strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", now_time);
    return std::string(str);
}

std::string HTTPResponse::generateAutoindexHTML(const Uri &uri, DIR *dir_p) const
{
    std::stringstream ss;

    ss << "<html>" << CRLF
       << "<head><title>Index of " << uri.getRawUri() << "</title></head>" << CRLF
       << "<body>" << CRLF
       << "<h1>Index of " << uri.getRawUri()
       << "</h1><hr><pre style=\"font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, 'Liberation Mono', 'Courier New', monospace\"><a href=\"../\">../</a>"
       << CRLF;

    for (struct dirent *entry = readdir(dir_p); entry != NULL; entry = readdir(dir_p))
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        appendFileHTML(ss, uri, entry);
    }

    ss << "</pre><hr></body>" << CRLF
       << "</html>" << CRLF;

    return ss.str();
}

void HTTPResponse::appendFileHTML(std::stringstream &ss,
                                  const Uri &uri, const struct dirent *entry) const
{
    std::string path = uri.getPath() + entry->d_name;
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) < 0)
    {
        return;
    }

    std::string href = generateHref(entry);
    std::string filename = generateFilename(entry);
    std::string mtime = generateMtime(&path_stat);
    size_t mtime_width = AUTOINDEX_FILENAME_WIDTH + 1 - filename.size() + mtime.size();

    ss << "<a href=\"" << escapeUri(href) << "\">"
       << escapeHTML(filename) << "</a>"
       << std::setw(mtime_width) << mtime
       << std::setw(AUTOINDEX_FILESIZE_WIDTH);

    if (S_ISDIR(path_stat.st_mode))
    {
        ss << "-" << CRLF;
    }
    else
    {
        ss << path_stat.st_size << CRLF;
    }
}

std::string HTTPResponse::generateHref(const struct dirent *entry) const
{
    std::string filename = std::string(entry->d_name);
    if (entry->d_type == DT_DIR)
    {
        filename.append("/");
    }
    return filename;
}

std::string HTTPResponse::generateFilename(const struct dirent *entry) const
{
    std::string filename = generateHref(entry);
    if (filename.size() > AUTOINDEX_FILENAME_WIDTH)
    {
        filename = filename.substr(0, AUTOINDEX_FILENAME_WIDTH - 3).append("..>");
    }
    return filename;
}

std::string HTTPResponse::generateMtime(const struct stat *path_stat) const
{
    // Format: "01-Jan-1970 00:00"
    char mtime_str[MTIME_STR_LEN];

    struct tm *mtime = gmtime(&path_stat->st_mtimespec.tv_sec);
    strftime(mtime_str, MTIME_STR_LEN, "%d-%b-%Y %H:%M", mtime);
    return std::string(mtime_str);
}

std::string HTTPResponse::escapeHTML(const std::string &str) const
{
    std::string escaped_str = str;
    size_t escaped_index = 0;

    for (size_t i = 0; i < str.size(); i++, escaped_index++)
    {
        switch (str.at(i))
        {
        case '<':
            escaped_str.replace(escaped_index, 1, "&lt;");
            escaped_index += 3;
            break;
        case '>':
            escaped_str.replace(escaped_index, 1, "&gt;");
            escaped_index += 3;
            break;
        case '&':
            escaped_str.replace(escaped_index, 1, "&amp;");
            escaped_index += 4;
            break;
        case '"':
            escaped_str.replace(escaped_index, 1, "&quot;");
            escaped_index += 5;
            break;
        default:
            break;
        }
    }
    return escaped_str;
}

std::string HTTPResponse::escapeUri(const std::string &str) const
{
    std::stringstream ss;
    std::string escape_chars = " \"<>\\^`{|}#%&+;?";

    for (size_t i = 0; i < str.size(); i++)
    {
        char c = str.at(i);
        if ((0x0 <= c && c <= 0x1F) || (0x7F <= c) ||
            escape_chars.find(c) != std::string::npos)
        {
            ss << "%" << std::hex << (int)c;
        }
        else
        {
            ss << c;
        }
    }
    return ss.str();
}
