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
    const char *str_port = sstream.str().c_str();

    struct addrinfo hints = {};

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

    struct addrinfo *addr_list;
    struct addrinfo *current;

    int code = getaddrinfo(NULL, str_port, &hints, &addr_list);
    if (code != 0)
    {
        throw AddressInfoError("getaddrinfo", code);
    }
    for (current = addr_list; current; current = current->ai_next)
    {
        fd_ = open_from_address(current);
        if (fd_ >= 0)
        {
            break;
        }
    }
    if (current == NULL)
    {
        throw std::runtime_error("socket not found");
    }
    struct sockaddr_storage *storage_addr = reinterpret_cast<struct sockaddr_storage *>(current->ai_addr);
    address_ = *storage_addr;
    freeaddrinfo(addr_list);
}

int ServerSocket::open_from_address(struct addrinfo *address)
{
    int fd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (fd < 0)
    {
        return -1;
    }

    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
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

int ServerSocket::accept_connection()
{
    int connect_d = accept(fd_, NULL, 0);

    if (connect_d < 0)
    {
        throw SystemError("accept", errno);
    }
    return connect_d;
}
