#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "socketwrapper.h"

#define	true		1
#define false		0

#define MYPORT 		3490
#define MAX_DATASIZE 1024
#define CMD_LENGTH 128

struct Message
{
    int m_size;
    char m_data[MAX_DATASIZE];
} msg;

struct Command
{
    char m_text[CMD_LENGTH];
} cmd;

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
	serverAddr.sin_port = Htons(MYPORT);
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

    Close(sockfd);
	return true;
}

int SendFile(int fd, struct sockaddr_in remoteAddr, const char *filename)
{
    FILE* fp = fopen(filename, "rb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    while(true)
    {
        msg.m_size = fread(msg.m_data, 1, MAX_DATASIZE, fp);
        Sendto(fd, &msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
        if(msg.m_size < MAX_DATASIZE)
        {
            break;
        }
    }

    fclose(fp);
    return true;
}
















