#ifndef _TCP_HANDLER_H_
#define _TCP_HANDLER_H_

#include "Main.h"

#define SERVER      0
#define CLIENT      1

#define IP_ADDR     "127.0.0.1"
#define PORT        3490

int BuildConnect(int type);
int SendBlock(Block *pBlock);
int RecvBlock(Block *pBlock);
int SendReq(char *str);
int RecvReq(char *buf);
int CloseConnect(int type);

#endif
