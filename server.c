#include "TcpHandler.h"
#include "FileHandler.h"
#include "Main.h"

Block buf[BLOCKS_IN_BUF];
sem_t semEmpty, semFull;

int main(int argc, char *argv[])
{
    char filename[REQ_LENGTH];

    if(BuildConnect(SERVER) == false)
    {
        return -1;
    }

    RecvReq(filename);
    printf("Start sending: %s\n", filename);
    if(SendFile(filename) == false)
    {
        CloseConnect(SERVER);
        return -1;
    }

    printf("trans OK;\n"); 
    CloseConnect(SERVER);
    return 0;
}

int SendFile(char *filename)
{
    int pos = 0;
    int nBytes;
    pthread_t pid;

    if(OpenFile(filename, READ) == false)
    {
        return false;
    }
    sem_init(&semEmpty, 0, BLOCKS_IN_BUF);
    sem_init(&semFull, 0, 0);
    pthread_create(&pid, NULL, ThreadFileReader, NULL);

    do
    {
        sem_wait(&semFull);
        nBytes = SendBlock(&buf[pos]);
        sem_post(&semEmpty);
        pos = (pos + 1) % BLOCKS_IN_BUF;

    } while(nBytes == BYTES_IN_BLOCK);

    pthread_join(pid, NULL);
    CloseFile();
    return true;
}

void* ThreadFileReader()
{
    int pos = 0;
    int nBytes;
    do
    {
        sem_wait(&semEmpty);
        nBytes = ReadBlock(&buf[pos]);
        sem_post(&semFull);
        pos = (pos + 1) % BLOCKS_IN_BUF;

    } while(nBytes == BYTES_IN_BLOCK);
}
