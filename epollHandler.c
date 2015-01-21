#include <sys/epoll.h>
#include "fileSender.h"
#include "epollHandler.h"

struct epoll_event ev, myev;
int epfd;

int EpollInit(int fd)
{
    epfd = epoll_create(1);
    if(epfd == -1)
    {
        perror("epoll_create");
        return false;
    }

    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    return true;
}

int EpollWait(int timeout)
{
    while(epoll_wait(epfd, &myev, 1, timeout) > 0)
    {
        if(myev.events & EPOLLIN)
        {
            return true;
        }
    }

    return false;
}
