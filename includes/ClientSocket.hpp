#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <string>
#include "Socket.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPParser.hpp"
#include "ServerConfig.hpp"
#include "KqueuePoller.hpp"

class ClientSocket : public Socket
{
public:
    enum State
    {
        READ_REQUEST,
        WRITE_RESPONSE,
        READ_FILE,
        CLOSE
    };

    ClientSocket(int fd, const struct sockaddr_storage &address,
                 const ServerConfig &config, const KqueuePoller &poller);
    ~ClientSocket();
    void receiveRequest();
    void sendResponse();
    void readFile(intptr_t offset);
    void closeFile();
    void close();
    State getState() const;

private:
    static const size_t BUF_SIZE;
    const ServerConfig &config_;
    const KqueuePoller &poller_;
    HTTPRequest request_;
    HTTPResponse response_;
    HTTPParser parser_;
    State state_;
    int file_fd_;

    void changeState(State new_state);
    void prepareResponse();
    void handleGET();
    void openFile(const char *path);
    DIR *openDirectory(const char *path);
    void closeDirectory(DIR *dir_p);
    void clearRequest();
    const LocationConfig *searchLocationConfig(const std::string &location);
};

#endif /* CLIENTSOCKET_HPP */
