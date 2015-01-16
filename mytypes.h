#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_

#define true        1
#define false       0

#define PORT        3490
#define MAX_DATASIZE 1024
#define CMD_LENGTH 128
#define EPOLL_SIZE 1
#define TIMEOUT 100
#define MSG_QUEUE_SIZE 10000

typedef struct
{
    int m_order;
    int m_size;
    char m_data[MAX_DATASIZE];
} PackMsg;

typedef struct
{
    int m_order;
} PackAck;

typedef struct
{
    char m_text[CMD_LENGTH];
} PackCmd;

#endif
