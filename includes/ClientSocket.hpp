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
    void readCGI(intptr_t offset);
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
    void openFile(const char *path);
    DIR *openDirectory(const char *path);
    void createPipe(const std::string &method, int *pipe_cgi_read, int *pipe_cgi_write);
    void prepareCGIInOut(const std::string &method, int *pipe_cgi_read, int *pipe_cgi_write);
    void prepareServerInOut(const std::string &method, int *pipe_cgi_read, int *pipe_cgi_write);
    void closeDirectory(DIR *dir_p);
    void clearRequest();
    const LocationConfig *searchLocationConfig(const std::string &location);
    std::string resolveIPAddress(const sockaddr_storage &addr) const;
};

#endif /* CLIENTSOCKET_HPP */
