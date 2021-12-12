#ifndef KQUEUEPOLLER_HPP
#define KQUEUEPOLLER_HPP

#include <sys/event.h>
#include "Socket.hpp"

class KqueuePoller
{
public:
    KqueuePoller();
    ~KqueuePoller();
    void registerServerSocket(Socket *socket);
    void registerClientSocket(Socket *socket);
    int watchEvents();
    const struct kevent *getEvents();

private:
    static const int MAX_EVENT_SIZE;
    int kqueue_d_;
    struct kevent *events_;
};

#endif /* KQUEUEPOLLER_HPP */
