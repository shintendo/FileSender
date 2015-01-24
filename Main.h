#ifndef _MAIN_H_
#define _MAIN_H_

#define true                1
#define false               0

#define BYTES_IN_BLOCK      200000
#define BLOCKS_IN_BUF       100
#define BUFSIZE             1000000
#define REQ_LENGTH          100

typedef struct
{
    int m_size;
    char m_data[BYTES_IN_BLOCK];
} Block;

void* ThreadFileWriter();
void* ThreadFileReader();

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#endif
