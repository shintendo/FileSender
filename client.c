#include "fileSender.h"
#include "udpHandler.h"
#include "epollHandler.h"

PackMsg msg[MSGS_IN_BUF];
PackCmd cmd;
PackAck ack;
FILE *fp;
sem_t semEmpty, semFull;

void* ThreadFileWriter();

int main(int argc, char *argv[])
{
    time_t t_start, t_end;

    if(UdpInit(CLIENT_UDP) == false)
    {
        exit(1);
    }
    
    memset(&cmd, 0, sizeof(cmd));
    printf("input file name:\n");
    gets(cmd.m_text);

    t_start = time(NULL);

    UdpSend(&cmd, sizeof(cmd));
    if(RecvFile("file2.img") == false)
    {
        perror("RecvFile");
        UdpClose();
        exit(1);
    }

    t_end = time(NULL);

    printf("trans OK; time used: %.0fs\n", difftime(t_end, t_start));
    UdpClose();
    return 0;
}

int RecvFile(const char *filename)
{
    int seq = 0;
    int pos = 0;
    pthread_t pid;

    fp = fopen(filename, "wb");
    if(fp == 0)
    {
        perror("fopen");
        return false;
    }

    sem_init(&semEmpty, 0, MSGS_IN_BUF);
    sem_init(&semFull, 0, 0);

    if(pthread_create(&pid, NULL, ThreadFileWriter, NULL) != 0)
    {
        perror("pthread_create");
        fclose(fp);
        return false;
    }

    while(true)
    {
        sem_wait(&semEmpty);
        RecvMsg(seq++, &msg[pos]);
        sem_post(&semFull);

        if(msg[pos].m_size < BYTES_IN_MSG)
        {
            break;
        }

        pos = (pos + 1) % MSGS_IN_BUF;
    }

    pthread_join(pid, NULL);
    fclose(fp);
    return true;
}

int RecvMsg(int seq, PackMsg *p_msg)
{
    do
    {
        UdpRecv(p_msg, sizeof(PackMsg));
        ack.m_seq = p_msg->m_seq;
        UdpSend(&ack, sizeof(PackAck));
    } while(p_msg->m_seq != seq);

    return p_msg->m_size;
}

void* ThreadFileWriter()
{
    int pos = 0;
    while(true)
    {
        sem_wait(&semFull);
        fwrite(msg[pos].m_data, 1, msg[pos].m_size, fp);
        sem_post(&semEmpty);

        if(msg[pos].m_size < BYTES_IN_MSG)
        {
            break;
        }

        pos = (pos + 1) % MSGS_IN_BUF;
    }
}
