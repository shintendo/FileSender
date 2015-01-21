#ifndef _EPOLL_HANDLER_H_
#define _EPOLL_HANDLER_H_

int EpollInit(int fd);
int EpollWait(int timeout);

#endif