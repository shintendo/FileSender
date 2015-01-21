#ifndef _UDP_HANDLER_H_
#define _UDP_HANDLER_H_

#define SERVER_UDP 0
#define CLIENT_UDP 1

int UdpInit(int type);
int UdpRecv(void *buf, int size);
int UdpSend(void* buf, int size);
int UdpSockfd();
void UdpClose();

#endif