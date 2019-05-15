#ifndef __U2GBK_H
#define __U2GBK_H

#include "sys.h"

int GetUtf8ByteNumForWord(u8 firstCh);
void Utf8ToGb2312(const char* utf8, int len, u8 *temp);



#endif

