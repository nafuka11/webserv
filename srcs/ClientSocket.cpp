#include "ClientSocket.hpp"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include "SystemError.hpp"
#include "HTTPParseException.hpp"
#include "Uri.hpp"

const size_t ClientSocket::BUF_SIZE = 8192;

ClientSocket::ClientSocket(int fd, const struct sockaddr_storage &address,
                           const ServerConfig &config, const KqueuePoller &poller)
    : Socket(CLIENT, fd), config_(config), poller_(poller),
      parser_(request_, config_), state_(READ_REQUEST)
{
    address_ = address;
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
        changeState(WRITE_RESPONSE);
        response_.setStatusCode(e.getStatusCode());
    }
}

void ClientSocket::changeState(State new_state)
{
    switch (state_)
    {
    case READ_REQUEST:
        poller_.unregisterReadEvent(this, fd_);
        break;
    case READ_FILE:
        break;
    case WRITE_RESPONSE:
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
        poller_.registerReadEvent(this, file_fd_);
        break;
    case WRITE_RESPONSE:
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
    switch (request_.getMethod())
    {
    case GET:
        handleGET();
        break;
    default:
        break;
    }
}

void ClientSocket::handleGET()
{
    Uri uri = Uri(config_, request_.getUri());
    std::string path = uri.getPath();

    if (uri.getNeedAutoIndex())
    {
        DIR *dir_p = openDirectory(path.c_str());
        std::string body = response_.generateAutoindexHTML(uri, dir_p);
        response_.appendMessageBody(body.c_str());
        closeDirectory(dir_p);
        changeState(WRITE_RESPONSE);
        return;
    }

    openFile(path.c_str());
    setNonBlockingFd(file_fd_);
    changeState(READ_FILE);
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

void ClientSocket::readFile(intptr_t offset)
{
    char buffer[BUF_SIZE];
    ssize_t read_byte = read(file_fd_, buffer, BUF_SIZE - 1);
    // TODO: handle read_byte < 0
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

void ClientSocket::closeFile()
{
    ::close(file_fd_);
    changeState(WRITE_RESPONSE);
}

void ClientSocket::sendResponse()
{
    response_.setKeepAlive(request_.canKeepAlive());
    std::string message = response_.toString();
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

void ClientSocket::clearRequest()
{
    request_.clear();
    parser_.clear();
}
