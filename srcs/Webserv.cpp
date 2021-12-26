#include "Webserv.hpp"
#include "ServerSocket.hpp"

Webserv::Webserv(const std::string &filepath) : config_(Config())
{
    config_.readFile(filepath);
    setupServers();
    registerEvents();
}

Webserv::~Webserv()
{
}

void Webserv::run()
{
    while (true)
    {
        watchEvents();
    }
}

void Webserv::setupServers()
{
    for (size_t i = 0; i < config_.server().size(); i++)
    {
        ServerSocket *server = new ServerSocket(config_.server()[i]);
        sockets_.insert(std::make_pair(server->getFd(), server));
    }
}

void Webserv::registerEvents()
{
    for (std::map<int, Socket *>::iterator iter = sockets_.begin();
         iter != sockets_.end(); ++iter)
    {
        poller_.registerReadEvent(iter->second, iter->second->getFd());
    }
}

void Webserv::watchEvents()
{
    int num_event = poller_.watchEvents();
    const struct kevent *events = poller_.getEvents();
    for (int i = 0; i < num_event; i++)
    {
        Socket *socket = reinterpret_cast<Socket *>(events[i].udata);
        switch (socket->getType())
        {
        case Socket::CLIENT:
            handleClientEvent(socket, events[i]);
            break;
        case Socket::SERVER:
            handleServerEvent(socket, events[i]);
            break;
        }
    }
}

void Webserv::handleServerEvent(Socket *socket, const struct kevent &event)
{
    if (event.filter == EVFILT_READ)
    {
        ServerSocket *server = dynamic_cast<ServerSocket *>(socket);
        ClientSocket *client = server->acceptConnection(poller_);
        sockets_.insert(std::make_pair(client->getFd(), client));
        poller_.registerReadEvent(client, client->getFd());
    }
}

void Webserv::handleClientEvent(Socket *socket, const struct kevent &event)
{
    ClientSocket *client = dynamic_cast<ClientSocket *>(socket);

    switch (client->getState())
    {
    case ClientSocket::READ_REQUEST:
        if (event.flags & EV_EOF)
        {
            closeClient(client);
            return;
        }
        if (event.filter == EVFILT_READ)
        {
            client->receiveRequest();
        }
        break;
    case ClientSocket::READ_FILE:
        if (event.flags & EV_EOF)
        {
            client->closeFile();
        }
        if (event.filter == EVFILT_READ)
        {
            client->readFile(event.data);
        }
        break;
    case ClientSocket::WRITE_RESPONSE:
        if (event.filter == EVFILT_WRITE)
        {
            client->sendResponse();
        }
        break;
    default:
        break;
    }
    if (client->getState() == ClientSocket::CLOSE)
    {
        closeClient(client);
    }
}

void Webserv::closeClient(ClientSocket *client)
{
    sockets_.erase(client->getFd());
    client->close();
}
