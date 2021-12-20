#include "HTTPResponse.hpp"
#include <sstream>

const std::map<HTTPStatusCode, std::string> HTTPResponse::REASON_PHRASE = HTTPResponse::setReasonPhrase();
const size_t HTTPResponse::DATE_STR_LEN = 40;

HTTPResponse::HTTPResponse()
{
}

HTTPResponse::~HTTPResponse()
{
}

std::string HTTPResponse::toString()
{
    setProperties();

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

void HTTPResponse::setStatusCode(HTTPStatusCode status_code)
{
    status_code_ = status_code;
}

void HTTPResponse::setKeepAlive(bool keep_alive)
{
    keep_alive_ = keep_alive;
}

std::map<HTTPStatusCode, std::string> HTTPResponse::setReasonPhrase()
{
    std::map<HTTPStatusCode, std::string> reason_phrase;

    reason_phrase[CODE_200] = "OK";
    reason_phrase[CODE_400] = "Bad Request";
    reason_phrase[CODE_404] = "Not Found";
    reason_phrase[CODE_413] = "Payload Too Large";
    reason_phrase[CODE_501] = "Not Implemented";
    return reason_phrase;
}

void HTTPResponse::setProperties()
{
    message_body_ = generateHTMLfromStatusCode();
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
}

std::string HTTPResponse::generateHTMLfromStatusCode() const
{
    std::stringstream ss;

    std::string phrase = HTTPResponse::REASON_PHRASE.find(status_code_)->second;
    ss << "<html>"
       << "\r\n"
       << "<head><title>" << status_code_ << " " << phrase << "</title></head>"
       << "\r\n"
       << "<body>"
       << "\r\n"
       << "<center><h1>" << status_code_ << " " << phrase << "</h1></center>"
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
