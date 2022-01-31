#include "ClientSocket.hpp"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include "AddressInfoError.hpp"
#include "Uri.hpp"
#include <iostream> //TODO: 後で消す

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address,
                           const ServerConfig &config, const KqueuePoller &poller)
    : Socket(CLIENT, fd), config_(config), poller_(poller),
      parser_(request_, config_), cgi_parser_(request_, config_), state_(READ_REQUEST)
{
    address_ = address;
    ip_ = resolveIPAddress(address_);
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::receiveRequest()
{
    char buffer[BUF_SIZE];
    ssize_t read_byte = recv(fd_, buffer, BUF_SIZE - 1, 0);
    if (read_byte <= 0)
    {
        changeState(CLOSE);
        clearRequest();
        return;
    }
    buffer[read_byte] = '\0';
    parser_.appendRawMessage(buffer);
    try
    {
        parser_.parse();
        if (parser_.finished())
        {
            prepareResponse();
        }
    }
    catch (const HTTPParseException &e)
    {
        handleError(e.getStatusCode());
    }
}

void ClientSocket::sendResponse()
{
    response_.setKeepAlive(request_.canKeepAlive());
    std::string message = response_.toString(searchLocationConfig(request_.getLocation()));
    ::send(fd_, message.c_str(), message.size(), 0);
    if (request_.canKeepAlive())
    {
        changeState(READ_REQUEST);
        response_.clear();
    }
    else
    {
        changeState(CLOSE);
    }
    clearRequest();
}

void ClientSocket::sendCGIResponse()
{
    cgi_parser_.parse(response_);
    response_.setKeepAlive(request_.canKeepAlive());
    // std::string message = response_.CGItoString(searchLocationConfig(request_.getLocation()));
    std::string message = response_.toString(searchLocationConfig(request_.getLocation()));
    ::send(fd_, message.c_str(), message.size(), 0);
    if (request_.canKeepAlive())
    {
        changeState(READ_REQUEST);
        response_.clear();
    }
    else
    {
        changeState(CLOSE);
    }
    request_.clear();
    cgi_parser_.clear();
}

void ClientSocket::readFile(intptr_t offset)
{
    char buffer[BUF_SIZE];
    ssize_t read_byte = read(file_fd_, buffer, BUF_SIZE - 1);

    if (read_byte < 0)
    {
        response_.setStatusCode(CODE_404);
    }
    if (read_byte <= 0)
    {
        closeFile();
        return;
    }
    buffer[read_byte] = '\0';
    response_.appendMessageBody(buffer);
    if (read_byte == offset)
    {
        closeFile();
    }
}

void ClientSocket::readCGI(intptr_t offset)
{
    char buffer[BUF_SIZE];
    ssize_t read_byte = read(file_fd_, buffer, BUF_SIZE - 1);

    if (read_byte < 0)
    {
        response_.setStatusCode(CODE_404);
    }
    if (read_byte <= 0)
    {
        ::close(file_fd_);
        changeState(WRITE_CGI_RESPONSE);
        return;
    }
    buffer[read_byte] = '\0';
    cgi_parser_.appendRawMessage(buffer);
    if (read_byte == offset)
    {
        ::close(file_fd_);
        changeState(WRITE_CGI_RESPONSE);
    }
}

void ClientSocket::closeFile()
{
    ::close(file_fd_);
    changeState(WRITE_RESPONSE);
}

void ClientSocket::close()
{
    if (::close(fd_) < 0)
    {
        throw SystemError("close", errno);
    }
}

ClientSocket::State ClientSocket::getState() const
{
    return state_;
}

void ClientSocket::changeState(State new_state)
{
    switch (state_)
    {
    case READ_REQUEST:
        poller_.unregisterReadEvent(this, fd_);
        break;
    case READ_FILE:
    case READ_CGI:
        break;
    case WRITE_RESPONSE:
    case WRITE_CGI_RESPONSE:
        poller_.unregisterWriteEvent(this, fd_);
        break;
    default:
        break;
    }
    switch (new_state)
    {
    case READ_REQUEST:
        poller_.registerReadEvent(this, fd_);
        break;
    case READ_FILE:
    case READ_CGI:
        poller_.registerReadEvent(this, file_fd_);
        break;
    case WRITE_RESPONSE:
    case WRITE_CGI_RESPONSE:
        poller_.registerWriteEvent(this, fd_);
        break;
    default:
        break;
    }
    state_ = new_state;
}

void ClientSocket::prepareResponse()
{
    response_.setStatusCode(CODE_200);

    Uri uri = Uri(config_, request_.getUri(), request_.getMethod());

    switch (uri.getResourceType())
    {
    case Uri::FILE:
        handleFile(request_.getMethod(), uri);
        break;
    case Uri::AUTOINDEX:
        handleAutoindex(request_.getMethod(), uri);
        break;
    case Uri::REDIRECT:
        break;
    case Uri::CGI:
        handleCGI(request_.getMethod(), uri);
        break;
    default:
        break;
    }
}

void ClientSocket::handleFile(const std::string &method, const Uri &uri)
{
    if (method == HTTPRequest::HTTP_GET)
    {
        std::string path = uri.getLocalPath();

        openFile(path.c_str());
        setNonBlockingFd(file_fd_);
        changeState(READ_FILE);
    }
    else if (method == HTTPRequest::HTTP_DELETE)
    {
        std::string raw_uri = uri.getRawPath();
        if (uri.isDirectory(uri.getStat()) || !uri.canWrite(uri.getStat()))
        {
            throw HTTPParseException(CODE_403);
        }
        int unlink_result = unlink(uri.getLocalPath().c_str());
        if (unlink_result != 0)
        {
            throw HTTPParseException(CODE_500);
        }
        response_.setStatusCode(CODE_204);
        changeState(WRITE_RESPONSE);
    }
}

void ClientSocket::handleAutoindex(const std::string &method, const Uri &uri)
{
    if (method == HTTPRequest::HTTP_GET)
    {
        std::string path = uri.getLocalPath();

        DIR *dir_p = openDirectory(path.c_str());
        std::string body = response_.generateAutoindexHTML(uri, dir_p);
        response_.appendMessageBody(body.c_str());
        closeDirectory(dir_p);
        changeState(WRITE_RESPONSE);
    }
}

void ClientSocket::handleRedirect(const std::string &method, const Uri &uri)
{
    (void)method;
    (void)uri;
}

void ClientSocket::handleCGI(const std::string &method, const Uri &uri)
{
    (void)method;

    int pipe_fd[2]; // pipeの情報はClientSocketで持ちたい
    if (pipe(pipe_fd) < 0)
    {
        throw SystemError("pipe", errno);
    }

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        throw SystemError("fork", errno);
    }
    if (pid == 0)  // Child prosess
    {
        ::close(pipe_fd[0]);
        ::close(STDOUT_FILENO);
        dup2(pipe_fd[1], STDOUT_FILENO);

        CGI cgi = CGI(request_, uri, config_, method, ip_);
        cgi.Execute();
    }
    else // Parent process
    {
        if (waitpid(pid, NULL, 0) != pid)
        {
            throw SystemError("waitpid", errno);
        }

        ::close(pipe_fd[1]);
        file_fd_ = pipe_fd[0];
        setNonBlockingFd(file_fd_);
        changeState(READ_CGI);
    }
}

void ClientSocket::handleError(HTTPStatusCode statusCode)
{
    response_.setStatusCode(statusCode);
    const LocationConfig *location = searchLocationConfig(request_.getLocation());
    if (location)
    {
        try
        {
            handleErrorFromFile(location, statusCode);
            return;
        }
        catch (const HTTPParseException &e)
        {
            // 例外をcatchした場合は後続のsetMessageBodyに進むため、catch内では何もしない
        }
    }
    response_.setMessageBody(response_.generateHTMLfromStatusCode(statusCode));
    changeState(WRITE_RESPONSE);
}

void ClientSocket::handleErrorFromFile(const LocationConfig *location,
                                       HTTPStatusCode statusCode)
{
    const std::map<int, std::string> error_pages = location->errorPage();
    std::map<int, std::string>::const_iterator page_found = error_pages.find(statusCode);
    if (page_found != error_pages.end())
    {
        Uri uri = Uri(config_, page_found->second, HTTPRequest::HTTP_GET);
        handleFile(HTTPRequest::HTTP_GET, uri);
    }
    else
    {
        throw HTTPParseException(CODE_404);
    }
}

void ClientSocket::openFile(const char *path)
{
    file_fd_ = open(path, O_RDONLY);
    if (file_fd_ < 0)
    {
        throw HTTPParseException(CODE_404);
    }
}

DIR *ClientSocket::openDirectory(const char *path)
{
    DIR *dir_p = opendir(path);
    if (dir_p == NULL)
    {
        throw HTTPParseException(CODE_404);
    }
    return dir_p;
}

void ClientSocket::closeDirectory(DIR *dir_p)
{
    int result = closedir(dir_p);
    if (result < 0)
    {
        throw SystemError("closedir", errno);
    }
}

void ClientSocket::clearRequest()
{
    request_.clear();
    parser_.clear();
}

const LocationConfig *ClientSocket::searchLocationConfig(const std::string &location)
{
    const std::map<std::string, LocationConfig> &locations = config_.location();
    const std::map<std::string, LocationConfig>::const_iterator
        location_found = locations.find(location);
    if (location_found != locations.end())
    {
        return &(location_found->second);
    }
    return NULL;
}

std::string ClientSocket::resolveIPAddress(const sockaddr_storage &addr) const
{
    char hostname[NI_MAXHOST];
    int ret = getnameinfo(reinterpret_cast<const sockaddr *>(&addr), addr.ss_len,
                          hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    if (ret)
    {
        throw AddressInfoError("getnameinfo", ret);
    }
    return std::string(hostname);
}
