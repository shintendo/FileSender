#ifndef _FILE_HANDLER_H
#define _FILE_HANDLER_H

#include "Main.h"

#define READ        "rb"
#define WRITE       "wb"

int OpenFile(char *filename, char *type);
int WriteBlock(Block *pBlock);
int ReadBlock(Block *pBlock);
int CloseFile();

#endif
