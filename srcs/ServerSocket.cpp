#include "ServerSocket.hpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "AddressInfoError.hpp"
#include "SystemError.hpp"

ServerSocket::ServerSocket(int port) : Socket()
{
    port_ = port;
    open();
    listen();
}

ServerSocket::~ServerSocket()
{
}

void ServerSocket::open()
{
    std::stringstream sstream;
    sstream << port_;
    std::string string_port = sstream.str();

    struct addrinfo hints = {};

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

    struct addrinfo *addr_list;
    struct addrinfo *current;

    int code = getaddrinfo(NULL, string_port.c_str(), &hints, &addr_list);
    if (code != 0)
    {
        throw AddressInfoError("getaddrinfo", code);
    }
    for (current = addr_list; current; current = current->ai_next)
    {
        fd_ = openFromAddress(current);
        if (fd_ >= 0)
        {
            break;
        }
    }
    if (current == NULL)
    {
        throw std::runtime_error("socket not found");
    }
    struct sockaddr_storage storage = {};
    *reinterpret_cast<struct sockaddr *>(&storage) = *current->ai_addr;
    address_ = storage;
    freeaddrinfo(addr_list);
}

int ServerSocket::openFromAddress(struct addrinfo *address)
{
    int fd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (fd < 0)
    {
        return -1;
    }

    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
    {
        throw SystemError("setsockopt", errno);
    }
    if (bind(fd, address->ai_addr, address->ai_addrlen) == 0)
    {
        return fd;
    }
    if (close(fd) < 0)
    {
        throw SystemError("close", errno);
    }
    return -1;
}

void ServerSocket::listen()
{
    if (::listen(fd_, SOMAXCONN) < 0)
    {
        throw SystemError("listen", errno);
    }
}

ClientSocket ServerSocket::acceptConnection()
{
    struct sockaddr_storage address;
    socklen_t address_len = sizeof(struct sockaddr_storage);

    int connect_d = accept(fd_, reinterpret_cast<struct sockaddr *>(&address), &address_len);

    if (connect_d < 0)
    {
        throw SystemError("accept", errno);
    }
    ClientSocket clientSocket = ClientSocket(connect_d, address);
    return clientSocket;
}
