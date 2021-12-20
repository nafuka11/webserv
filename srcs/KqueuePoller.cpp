#include "KqueuePoller.hpp"
#include <cerrno>
#include "SystemError.hpp"

const int KqueuePoller::MAX_EVENT_SIZE = 32;

KqueuePoller::KqueuePoller()
{
    kqueue_d_ = kqueue();
    if (kqueue_d_ < 0)
    {
        throw SystemError("kqueue", errno);
    }
    events_ = new struct kevent[MAX_EVENT_SIZE];
}

KqueuePoller::~KqueuePoller()
{
    delete[] events_;
}

void KqueuePoller::registerReadEvent(Socket *socket, int fd) const
{
    struct kevent event_to_set;
    EV_SET(&event_to_set, fd,
           EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           socket);
    int result = kevent(kqueue_d_, &event_to_set, 1, NULL, 0, NULL);
    if (result < 0)
    {
        throw SystemError("kevent", errno);
    }
}
void KqueuePoller::registerWriteEvent(Socket *socket, int fd) const
{
    struct kevent event_to_set;
    EV_SET(&event_to_set, fd,
           EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           socket);
    int result = kevent(kqueue_d_, &event_to_set, 1, NULL, 0, NULL);
    if (result < 0)
    {
        throw SystemError("kevent", errno);
    }
}

void KqueuePoller::unregisterReadEvent(Socket *socket, int fd) const
{
    struct kevent event_to_delete;
    EV_SET(&event_to_delete, fd,
           EVFILT_READ, EV_DELETE, 0, 0, socket);
    int result = kevent(kqueue_d_, &event_to_delete, 1, NULL, 0, NULL);
    if (result < 0)
    {
        throw SystemError("kevent", errno);
    }
}

void KqueuePoller::unregisterWriteEvent(Socket *socket, int fd) const
{
    struct kevent event_to_delete;
    EV_SET(&event_to_delete, fd,
           EVFILT_WRITE, EV_DELETE, 0, 0, socket);
    int result = kevent(kqueue_d_, &event_to_delete, 1, NULL, 0, NULL);
    if (result < 0)
    {
        throw SystemError("kevent", errno);
    }
}

int KqueuePoller::watchEvents()
{
    int num_event = kevent(kqueue_d_, NULL, 0, events_, MAX_EVENT_SIZE, NULL);
    if (num_event < 0)
    {
        throw SystemError("kevent", errno);
    }
    return num_event;
}

const struct kevent *KqueuePoller::getEvents()
{
    return events_;
}
