#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "socketwrapper.h"
#include "mytypes.h"

PackMsg msg[MSG_QUEUE_SIZE];
PackCmd cmd;
PackAck ack;
FILE *fp;
sem_t semEmpty, semFull;

int RecvFile(int fd, struct sockaddr_in remoteAddr, const char *filename);
void* ThreadFileWriter();

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
    int pos;
    pthread_t pid;

    fp = fopen(filename, "wb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    sem_init(&semEmpty, 0, MSG_QUEUE_SIZE);
    sem_init(&semFull, 0, 0);

    if(pthread_create(&pid, NULL, ThreadFileWriter, NULL) != 0)
    {
        perror("pthread_create");
        fclose(fp);
        return false;
    }

    order = 0;
    pos = 0;
    while(true)
    {
        sem_wait(&semEmpty);
        Recvfrom(fd, &msg[pos], sizeof(PackMsg), 0, (struct sockaddr*)&remoteAddr, &addrlen);
        ack.m_order = msg[pos].m_order;
        Sendto(fd, &ack, sizeof(ack), 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));

        if(msg[pos].m_order != order)
        {
            sem_post(&semEmpty);
            continue;
        }
        sem_post(&semFull);

        if(msg[pos].m_size < MAX_DATASIZE)
        {
            break;
        }

        order = order + 1;
        pos = pos + 1;
        if(pos == MSG_QUEUE_SIZE)
        {
            pos = 0;
        }
    }

    pthread_join(pid, NULL);
    fclose(fp);
    return true;
}

void* ThreadFileWriter()
{
    int pos = 0;
    while(true)
    {
        sem_wait(&semFull);
        fwrite(msg[pos].m_data, 1, msg[pos].m_size, fp);
        sem_post(&semEmpty);

        if(msg[pos].m_size < MAX_DATASIZE)
        {
            break;
        }

        pos = pos + 1;
        if(pos == MSG_QUEUE_SIZE)
        {
            pos = 0;
        }
    }
}



