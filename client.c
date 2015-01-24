#include "TcpHandler.h"
#include "FileHandler.h"
#include "Main.h"

Block buf[BLOCKS_IN_BUF];
sem_t semEmpty, semFull;

int main()
{
    char filename[REQ_LENGTH];
    time_t t_start, t_end;

    if(BuildConnect(CLIENT) == false)
    {
        return -1;
    }
    printf("Input File Name:\n");
    gets(filename);

    t_start = time(NULL);
    SendReq(filename);
    if(RecvFile("new.file") == false)
    {
        CloseConnect(CLIENT);
        return -1;
    }
    t_end = time(NULL);

    printf("trans OK; time used: %.0fs\n", difftime(t_end, t_start));
    CloseConnect(CLIENT);
    return 0;
}

int RecvFile(char *filename)
{
    int pos = 0;
    int nBytes;
    pthread_t pid;

    if(OpenFile(filename, WRITE) == false)
    {
        return false;
    }
    sem_init(&semEmpty, 0, BLOCKS_IN_BUF);
    sem_init(&semFull, 0, 0);
    pthread_create(&pid, NULL, ThreadFileWriter, NULL);

    do
    {
        sem_wait(&semEmpty);
        nBytes = RecvBlock(&buf[pos]);
        sem_post(&semFull);
        pos = (pos + 1) % BLOCKS_IN_BUF;

    } while(nBytes == BYTES_IN_BLOCK);

    pthread_join(pid, NULL);
    CloseFile();
    return true;
}

void* ThreadFileWriter()
{
    int pos = 0;
    int nBytes;
    do
    {
        sem_wait(&semFull);
        nBytes = WriteBlock(&buf[pos]);
        sem_post(&semEmpty);
        pos = (pos + 1) % BLOCKS_IN_BUF;

    } while(nBytes == BYTES_IN_BLOCK);
}
