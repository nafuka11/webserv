#ifndef KQUEUEPOLLER_HPP
#define KQUEUEPOLLER_HPP

#include <vector>
#include <sys/event.h>
#include "Socket.hpp"

class KqueuePoller
{
public:
    KqueuePoller();
    ~KqueuePoller();
    void registerReadEvent(Socket *socket, int fd);
    void registerWriteEvent(Socket *socket, int fd);
    void unregisterReadEvent(Socket *socket, int fd);
    void unregisterWriteEvent(Socket *socket, int fd);
    int watchEvents();
    const struct kevent *getEvents();

private:
    static const int MAX_EVENT_SIZE;
    int kqueue_d_;
    struct kevent *events_;
    std::vector<struct kevent> changes_;
};

#endif /* KQUEUEPOLLER_HPP */
