#include "KqueuePoller.hpp"
#include <cerrno>
#include <unistd.h>
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
    close(kqueue_d_);
}

void KqueuePoller::registerReadEvent(Socket *socket, int fd)
{
    struct kevent event_to_set;
    EV_SET(&event_to_set, fd,
           EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           socket);
    changes_.push_back(event_to_set);
}

void KqueuePoller::registerWriteEvent(Socket *socket, int fd)
{
    struct kevent event_to_set;
    EV_SET(&event_to_set, fd,
           EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           socket);
    changes_.push_back(event_to_set);
}

void KqueuePoller::unregisterReadEvent(Socket *socket, int fd)
{
    struct kevent event_to_delete;
    EV_SET(&event_to_delete, fd,
           EVFILT_READ, EV_DELETE, 0, 0, socket);
    changes_.push_back(event_to_delete);
}

void KqueuePoller::unregisterWriteEvent(Socket *socket, int fd)
{
    struct kevent event_to_delete;
    EV_SET(&event_to_delete, fd,
           EVFILT_WRITE, EV_DELETE, 0, 0, socket);
    changes_.push_back(event_to_delete);
}

int KqueuePoller::watchEvents()
{
    struct kevent *changes = NULL;
    if (changes_.size())
    {
        changes = changes_.data();
    }
    int num_event = kevent(kqueue_d_, changes, changes_.size(),
                           events_, MAX_EVENT_SIZE, NULL);
    changes_.clear();
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
