#include "TcpHandler.h"

static int sockfd, fd;
static struct sockaddr_in serverAddr;
static int addrlen = sizeof(struct sockaddr_in);
static int bsize = BUFSIZE, sizelen = sizeof(int);

int BuildConnect(int type)
{
    memset(&serverAddr, 0, addrlen);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket");
        return false;
    }

    if(type == SERVER)
    {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &bsize, sizelen);
        if(bind(sockfd, (struct sockaddr *)&serverAddr, addrlen) != 0)
        {
            perror("bind");
            close(sockfd);
            return false;
        }
        if(listen(sockfd, 1) != 0)
        {
            perror("listen");
            close(sockfd);
            return false;
        }
        fd = accept(sockfd, NULL, NULL);
        if(fd == -1)
        {
            perror("accept");
            close(sockfd);
            return false;
        }
    }
    else
    {
        inet_aton(IP_ADDR, &serverAddr.sin_addr);
        setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &bsize, sizelen);
        if(connect(sockfd, (struct sockaddr *)&serverAddr, addrlen) != 0)
        {
            perror("connect");
            close(sockfd);
            return false;
        }
        fd = sockfd;
    }

    return true;
}

int SendBlock(Block *pBlock)
{
    send(fd, pBlock, sizeof(Block), 0);
    return pBlock->m_size;
}

int RecvBlock(Block *pBlock)
{
    recv(fd, pBlock, sizeof(Block), MSG_WAITALL);
    return pBlock->m_size;
}

int SendReq(char *str)
{
    send(fd, str, REQ_LENGTH, 0);
    return 0;
}

int RecvReq(char *buf)
{
    recv(fd, buf, REQ_LENGTH, MSG_WAITALL);
    return 0;
}

int CloseConnect(int type)
{
    close(fd);
    if(type == SERVER)
    {
        close(sockfd);
    }
    return 0;
}
