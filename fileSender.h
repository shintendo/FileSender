#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_

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

#define true                1
#define false               0

#define BYTES_IN_MSG        1024
#define MSGS_IN_BUF         10000
#define TIMEOUT             100

#define PORT                3490
#define CMD_LENGTH          128

typedef struct
{
    int m_seq;
    int m_size;
    char m_data[BYTES_IN_MSG];
} PackMsg;

typedef struct
{
    int m_seq;
} PackAck;

typedef struct
{
    char m_text[CMD_LENGTH];
} PackCmd;

#endif
