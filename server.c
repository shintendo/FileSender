#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "socketwrapper.h"
#include "mytypes.h"

PackMsg msg;
PackCmd cmd;
PackAck ack;

//for debug
int iret = 0, j=0;
int moni[1000000];

int main()
{
    int sockfd;
    int addrlen;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    
    if((sockfd = Socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = Htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(Bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) 
    {
        perror("bind");
        exit(1);
    }
    
    Recvfrom(sockfd, &cmd, sizeof(cmd), 0, (struct sockaddr*)&clientAddr, &addrlen);
    printf("start sending...\n");

    if(SendFile(sockfd, clientAddr, "file.img") == true)
    {
        printf("trans OK;\n");
    }

    printf("[debug info]pack missed:\n");
    for(j=0; j<iret; j++)
    {
        printf("%d\n", moni[j]);
    }

    Close(sockfd);
    return true;
}

int SendFile(int fd, struct sockaddr_in remoteAddr, const char *filename)
{
    struct epoll_event ev;
    int epfd;
    int order;
    int addrlen;
    int nfds;

    FILE* fp = fopen(filename, "rb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    epfd = epoll_create(EPOLL_SIZE);
    if(epfd == -1)
    {
        perror("epoll_create");
        fclose(fp);
        return false;
    }

    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    order = 0;
    while(true)
    {
        msg.m_size = fread(msg.m_data, 1, MAX_DATASIZE, fp);
        msg.m_order = order;
        Sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));

        while(WaitAck(epfd, fd, order, remoteAddr) == false)
        {
            moni[iret++] = msg.m_order;
            Sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
        }
        
        order = order + 1;

        if(msg.m_size < MAX_DATASIZE)
        {
            break;
        }
    }

    close(epfd);
    fclose(fp);
    return true;
}

int WaitAck(int epfd, int fd, int order, struct sockaddr_in remoteAddr)
{
    struct epoll_event events[EPOLL_SIZE];
    int addrlen;

    while(epoll_wait(epfd, events, EPOLL_SIZE, TIMEOUT) > 0)
    {
        if(events[0].events & EPOLLIN)
        {
            Recvfrom(fd, &ack, sizeof(ack), 0, (struct sockaddr*)&remoteAddr, &addrlen);
            if(ack.m_order == order)
            {
                return true;
            }
        }
    }

    return false;
}
















