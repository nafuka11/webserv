#ifndef KQUEUEPOLLER_HPP
#define KQUEUEPOLLER_HPP

#include <sys/event.h>
#include "Socket.hpp"

class KqueuePoller
{
public:
    KqueuePoller();
    ~KqueuePoller();
    void registerReadEvent(Socket *socket, int fd) const;
    void registerWriteEvent(Socket *socket, int fd) const;
    void unregisterReadEvent(Socket *socket, int fd) const;
    void unregisterWriteEvent(Socket *socket, int fd) const;
    int watchEvents();
    const struct kevent *getEvents();

private:
    static const int MAX_EVENT_SIZE;
    int kqueue_d_;
    struct kevent *events_;
};

#endif /* KQUEUEPOLLER_HPP */
