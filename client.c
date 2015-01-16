#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "socketwrapper.h"
#include "mytypes.h"

PackMsg msg;
PackCmd cmd;
PackAck ack;

int main(int argc, char *argv[])
{
	int sockfd;
	char *serverIP = "127.0.0.1";
	struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
    time_t t_start, t_end;
	
	if((sockfd=Socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = Htons(PORT);
    inet_aton(serverIP, &serverAddr.sin_addr);

    memset(&cmd, 0, sizeof(cmd));
    Sendto(sockfd, &cmd, sizeof(cmd), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("start receiving...\n");

    t_start = time(NULL);

    if(RecvFile(sockfd, serverAddr, "file2.img") == true)
    {
        t_end = time(NULL);
        printf("trans OK; time used: %.0fs\n", difftime(t_end, t_start));
    }

	Close(sockfd);
	return true;
}

int RecvFile(int fd, struct sockaddr_in remoteAddr, const char *filename)
{
    int addrlen;
    int order;

    FILE *fp = fopen(filename, "wb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    order = 0;
    while(true)
    {
        Recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, &addrlen);
        ack.m_order = msg.m_order;
        Sendto(fd, &ack, sizeof(ack), 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));

        if(msg.m_order != order)
        {
            continue;
        }

        fwrite(msg.m_data, 1, msg.m_size, fp);
        order = order + 1;

        if(msg.m_size < MAX_DATASIZE)
        {
            break;
        }
    }

    fclose(fp);
    return true;
}











