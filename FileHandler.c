#include "FileHandler.h"

static FILE *fp;

int OpenFile(char *filename, char *type)
{
    fp = fopen(filename, type);
    if(fp <= 0)
    {
        perror("fopen");
        return false;
    }
    return true;
}

int WriteBlock(Block *pBlock)
{
    fwrite(pBlock->m_data, 1, pBlock->m_size, fp);
    return pBlock->m_size;
}

int ReadBlock(Block *pBlock)
{
    int nBytes;
    nBytes = fread(pBlock->m_data, 1, BYTES_IN_BLOCK, fp);
    pBlock->m_size = nBytes;
    return nBytes;
}

int CloseFile()
{
    if(fp > 0)
    {
        fclose(fp);
    }
    return 0;
}
