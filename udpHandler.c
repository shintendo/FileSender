#include "fileSender.h"
#include "udpHandler.h"

int sockfd;
int addrlen = sizeof(struct sockaddr_in);
struct sockaddr_in remoteAddr;
struct sockaddr_in serverAddr;
char *serverIP = "127.0.0.1";

int UdpInit(int type)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
    {
        perror("socket");
        return false;
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if(type == CLIENT_UDP)
    {
        inet_aton(serverIP, &serverAddr.sin_addr);
        remoteAddr = serverAddr;
    }
    else
    {
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if(bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) 
        {
            perror("bind");
            close(sockfd);
            return false;
        }
    }
    return true;
}

int UdpRecv(void *buf, int size)
{
    int res = recvfrom(sockfd, buf, size, 0, (struct sockaddr*)&remoteAddr, &addrlen);
    return res;
}

int UdpSend(void* buf, int size)
{
    int res = sendto(sockfd, buf, size, 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
    return res;
}

int UdpSockfd()
{
    return sockfd;
}

void UdpClose()
{
    close(sockfd);
}