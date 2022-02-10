#include "ClientSocket.hpp"
#include <sstream>
#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include "AddressInfoError.hpp"
#include "Uri.hpp"

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address,
                           const std::vector<ServerConfig> &configs,
                           const KqueuePoller &poller)
    : Socket(CLIENT, fd), configs_(configs), poller_(poller),
      parser_(request_, configs_), cgi_parser_(request_, configs_, response_), state_(READ_REQUEST)
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
    parser_.appendRawMessage(buffer, read_byte);
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
    response_.setKeepAlive(request_.canKeepAlive());
    std::string message = response_.CGItoString(searchLocationConfig(request_.getLocation()));
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
    response_.appendMessageBody(buffer, read_byte);
    if (read_byte == offset)
    {
        closeFile();
    }
}

void ClientSocket::writeFile()
{
    const std::string &body = request_.getMessageBody();
    ssize_t write_byte = write(file_fd_, body.c_str(), body.size());
    if (write_byte < 0)
    {
        closeFile();
        handleError(CODE_500);
    }
    closeFile();
    response_.setStatusCode(CODE_201);
}

void ClientSocket::readCGI(intptr_t offset)
{
    char buffer[BUF_SIZE];
    ssize_t read_byte = read(file_fd_, buffer, BUF_SIZE - 1);

    if (read_byte < 0)
    {
        response_.setStatusCode(CODE_502);
    }
    if (read_byte != 0)
    {
        buffer[read_byte] = '\0';
        cgi_parser_.appendRawMessage(buffer, read_byte);
    }
    if ((read_byte == 0) || (read_byte == offset))
    {
        closeFile();
        try
        {
            cgi_parser_.parse();
        }
        catch (const HTTPParseException &e)
        {
            handleError(e.getStatusCode());
        }
    }
}

void ClientSocket::closeFile()
{
    ::close(file_fd_);

    switch (state_)
    {
    case READ_FILE:
        changeState(WRITE_RESPONSE);
        break;
    case READ_CGI:
        changeState(WRITE_CGI_RESPONSE);
        break;
    default:
        break;
    }
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
    case WRITE_FILE:
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
    case WRITE_FILE:
        poller_.registerWriteEvent(this, file_fd_);
    default:
        break;
    }
    state_ = new_state;
}

void ClientSocket::prepareResponse()
{
    response_.setStatusCode(CODE_200);

    Uri uri = Uri(*request_.getServerConfig(), request_.getUri(), request_.getMethod());

    switch (uri.getResourceType())
    {
    case Uri::FILE:
        handleFile(request_.getMethod(), uri);
        break;
    case Uri::AUTOINDEX:
        handleAutoindex(request_.getMethod(), uri);
        break;
    case Uri::REDIRECT:
        handleRedirect(uri);
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

        openFileToRead(path.c_str());
        setNonBlockingFd(file_fd_);
        changeState(READ_FILE);
        if (uri.getStat().st_size == 0)
        {
            closeFile();
        }
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
    else if (method == HTTPRequest::HTTP_POST)
    {
        if (uri.getLocationConfig()->uploadPath().empty())
        {
            throw HTTPParseException(CODE_403);
        }
        std::string filename = buildUploadFilename();
        std::string location = request_.getLocation() + filename;
        std::string path = uri.getLocationConfig()->uploadPath() + filename;

        response_.setHeader(std::make_pair("Location", location));

        openFileToWrite(path);
        setNonBlockingFd(file_fd_);
        changeState(WRITE_FILE);
    }
}

void ClientSocket::handleAutoindex(const std::string &method, const Uri &uri)
{
    if (method == HTTPRequest::HTTP_GET)
    {
        std::string path = uri.getLocalPath();

        DIR *dir_p = openDirectory(path.c_str());
        std::string body = response_.generateAutoindexHTML(uri, dir_p);
        response_.appendMessageBody(body.c_str(), body.size());
        closeDirectory(dir_p);
        changeState(WRITE_RESPONSE);
    }
}

void ClientSocket::handleRedirect(const Uri &uri)
{
    const std::map<int, std::string>
        redirects = uri.getLocationConfig()->returnRedirect();
    response_.setRedirectResponse(redirects.begin()->first, redirects.begin()->second);
    changeState(WRITE_RESPONSE);
}

void ClientSocket::handleCGI(const std::string &method, const Uri &uri)
{
    int pipe_cgi_read[2];
    int pipe_cgi_write[2];

    createPipe(method, pipe_cgi_read, pipe_cgi_write);

    pid_t pid = fork();
    if (pid < 0)
    {
        throw SystemError("fork", errno);
    }
    if (pid == 0)  // Child prosess
    {
        try
        {
            prepareCGIInOut(method, pipe_cgi_read, pipe_cgi_write);

            CGI cgi = CGI(request_, uri, *request_.getServerConfig(), method, ip_);
            cgi.execute();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        exit(EXIT_FAILURE);
    }
    else // Parent process
    {
        prepareServerInOut(method, pipe_cgi_read, pipe_cgi_write);

        if (method == "POST")
        {
            write(pipe_cgi_read[1], request_.getMessageBody().c_str(), parser_.getContentLength());
            closeFd(pipe_cgi_read[1]);
        }

        if (waitpid(pid, NULL, 0) != pid)
        {
            throw SystemError("waitpid", errno);
        }

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
        Uri uri = Uri(*request_.getServerConfig(), page_found->second, HTTPRequest::HTTP_GET);
        handleFile(HTTPRequest::HTTP_GET, uri);
    }
    else
    {
        throw HTTPParseException(CODE_404);
    }
}

void ClientSocket::openFileToRead(const std::string &path)
{
    file_fd_ = open(path.c_str(), O_RDONLY);
    if (file_fd_ < 0)
    {
        throw HTTPParseException(CODE_404);
    }
}

void ClientSocket::openFileToWrite(const std::string &path)
{
    file_fd_ = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd_ < 0)
    {
        throw HTTPParseException(CODE_403);
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

void ClientSocket::createPipe(const std::string &method,
                              int *pipe_cgi_read, int *pipe_cgi_write)
{
    if (method == "POST")
    {
        if (pipe(pipe_cgi_read) < 0)
        {
            throw SystemError("pipe", errno);
        }
    }
    if (pipe(pipe_cgi_write) < 0)
    {
        throw SystemError("pipe", errno);
    }
}

void ClientSocket::prepareCGIInOut(const std::string &method,
                                   int *pipe_cgi_read, int *pipe_cgi_write)
{
    if (method == "POST")
    {
        closeFd(pipe_cgi_read[1]);
        closeFd(STDIN_FILENO);
        duplicateFd(pipe_cgi_read[0], STDIN_FILENO);
    }
    closeFd(pipe_cgi_write[0]);
    closeFd(STDOUT_FILENO);
    duplicateFd(pipe_cgi_write[1], STDOUT_FILENO);
}

void ClientSocket::prepareServerInOut(const std::string &method,
                                      int *pipe_cgi_read, int *pipe_cgi_write)
{
    if (method == "POST")
    {
        closeFd(pipe_cgi_read[0]);
    }
    closeFd(pipe_cgi_write[1]);
    file_fd_ = pipe_cgi_write[0];
}

void ClientSocket::duplicateFd(int oldfd, int newfd)
{
    if (dup2(oldfd, newfd) < 0)
    {
        throw SystemError("dup2", errno);
    }
}

void ClientSocket::closeFd(int fd)
{
    if (::close(fd) < 0)
    {
        throw SystemError("close", errno);
    }
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
    cgi_parser_.clear();
}

const LocationConfig *ClientSocket::searchLocationConfig(const std::string &location)
{
    if (request_.getServerConfig() == NULL)
    {
        return NULL;
    }
    const std::map<std::string, LocationConfig> &
        locations = request_.getServerConfig()->location();
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

std::string ClientSocket::buildUploadFilename()
{
    time_t now = time(NULL);
    struct tm *now_time = gmtime(&now);
    char str[20];

    // ファイルフォーマット: yyyymmddHHMMSS_rand()
    strftime(str, sizeof(str), "%Y%m%d%H%M%S", now_time);
    std::stringstream ss;
    ss << str << "_" << rand();
    return ss.str();
}
