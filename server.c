#include "fileSender.h"
#include "udpHandler.h"
#include "epollHandler.h"

PackMsg msg[MSGS_IN_BUF];
PackCmd cmd;
PackAck ack;
FILE *fp;
sem_t semEmpty, semFull;

void* ThreadFileReader();

int main()
{
    if(UdpInit(SERVER_UDP) == false)
    {
        exit(1);
    }

    UdpRecv(&cmd, sizeof(cmd));
    if(SendFile(cmd.m_text) == false)
    {
        perror("SendFile");
        UdpClose();
    }

    printf("trans OK;\n");
    UdpClose();
    return true;
}

int SendFile(const char *filename)
{
    int seq = 0;
    int pos = 0;
    pthread_t pid;

    fp = fopen(filename, "rb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    if(EpollInit(UdpSockfd()) == false)
    {
        perror("EpollInit");
        fclose(fp);
        return false;
    }

    sem_init(&semEmpty, 0, MSGS_IN_BUF);
    sem_init(&semFull, 0, 0);

    if(pthread_create(&pid, NULL, ThreadFileReader, NULL) != 0)
    {
        perror("pthread_create");
        fclose(fp);
        return false;
    }

    while(true)
    {
        sem_wait(&semFull);
        SendMsg(seq++, &msg[pos]);

        if(msg[pos].m_size < BYTES_IN_MSG)
        {
            sem_post(&semEmpty);
            break;
        }
        sem_post(&semEmpty);
        
        pos = (pos + 1) % MSGS_IN_BUF;
    }

    pthread_join(pid, NULL);
    return true;
}

int SendMsg(int seq, PackMsg *p_msg)
{
    p_msg->m_seq = seq;
    do
    {
        UdpSend(p_msg, sizeof(PackMsg));
    } while(WaitAck(seq) == false);

    return p_msg->m_size;
}

int WaitAck(int seq)
{
    while(EpollWait(TIMEOUT) == true)
    {
        UdpRecv(&ack, sizeof(PackAck));
        if(ack.m_seq == seq)
        {
            return true;
        }
    }
    return false;
}

void* ThreadFileReader()
{
    int pos = 0;
    while(true)
    {
        sem_wait(&semEmpty);
        msg[pos].m_size = fread(msg[pos].m_data, 1, BYTES_IN_MSG, fp);
        sem_post(&semFull);

        if(msg[pos].m_size < BYTES_IN_MSG)
        {
            break;
        }

        pos = (pos + 1) % MSGS_IN_BUF;
    }
}