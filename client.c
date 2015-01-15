#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "dbtime.h"
#include "socketwrapper.h"

#define	true		1
#define false		0

#define PORT 		3490
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

int main(int argc, char *argv[])
{
	int sockfd;
	char *serverIP = "127.0.0.1";
	struct sockaddr_in clientAddr;
    struct sockaddr_in serverAddr;
	
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

	dbtime_startTest ("Connect & Recv");

    if(RecvFile(sockfd, serverAddr, "file2.img") == true)
    {
        printf("trans OK;\n");
    }
	
	dbtime_endAndShow ();

	Close(sockfd);
	
	dbtime_startTest ("Sleep 5s");
    sleep(5);
	dbtime_endAndShow ();
	dbtime_finalize ();

	return true;
}

int RecvFile(int fd, struct sockaddr_in remoteAddr, const char *filename)
{
	struct Message msg;
    int addrlen;

    FILE *fp = fopen(filename, "wb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    while(true)
    {
        Recvfrom(fd, &msg, sizeof(msg), 0, (struct sockaddr*)&remoteAddr, &addrlen);
        fwrite(msg.m_data, 1, msg.m_size, fp);
        if(msg.m_size < MAX_DATASIZE)
        {
            break;
        }
    }

    fclose(fp);
    return true;
}











