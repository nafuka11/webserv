#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPParser.hpp"
#include "ServerConfig.hpp"
#include "KqueuePoller.hpp"
#include "CGI.hpp"
#include "CGIParser.hpp"

class ClientSocket : public Socket
{
public:
    enum State
    {
        READ_REQUEST,
        READ_FILE,
        READ_CGI,
        WRITE_RESPONSE,
        WRITE_CGI_RESPONSE,
        WRITE_FILE,
        WRITE_TO_CGI,
        CLOSE
    };

    ClientSocket(int fd, const struct sockaddr_storage &address,
                 const std::vector<ServerConfig> &configs,
                 const KqueuePoller &poller);
    ~ClientSocket();
    void receiveRequest();
    void sendResponse();
    void sendCGIResponse();
    void readFile(intptr_t offset);
    void writeFile();
    void readCGI(intptr_t offset);
    void writeToCGI();
    void closeFile();
    void close();
    State getState() const;

private:
    static const size_t BUF_SIZE;
    const std::vector<ServerConfig> &configs_;
    const KqueuePoller &poller_;
    HTTPRequest request_;
    HTTPResponse response_;
    HTTPParser parser_;
    CGIParser cgi_parser_;
    CGI cgi_;
    State state_;
    int file_fd_;
    std::string ip_;

    void changeState(State new_state);
    void prepareResponse();
    void handleFile(const std::string &method, const Uri &uri);
    void handleAutoindex(const std::string &method, const Uri &uri);
    void handleRedirect(const Uri &uri);
    void handleCGI(const std::string &method, const Uri &uri);
    void handleError(HTTPStatusCode statusCode);
    void handleErrorFromFile(const LocationConfig *location, HTTPStatusCode statusCode);
    void openFileToRead(const std::string &path);
    void openFileToWrite(const std::string &path);
    DIR *openDirectory(const char *path);
    void closeDirectory(DIR *dir_p);
    void clearRequest();
    const LocationConfig *searchLocationConfig(const std::string &location);
    std::string resolveIPAddress(const sockaddr_storage &addr) const;
    std::string buildUploadFilename();
};

#endif /* CLIENTSOCKET_HPP */
