#include "cp936.h"
#include "usart.h"
#include "U2GBK.h"
#include <string.h>
#include "text.h"	


int GetUtf8ByteNumForWord(u8 firstCh)
{
	u8 temp = 0x80;
	int num = 0;

	while (temp & firstCh)
	{
		num++;
		temp = (temp >> 1);
	}

	printf("the num is: %d\r\n", num);
	return num;
}

//#define CODE_TABLE_SIZE 21791
u16 SearchCodeTable(u16 unicodeKey)
{
    int first = 0;
    int end = CODE_TABLE_SIZE - 1;
    int mid = 0;

    while (first <= end)
    {
        mid = (first + end) / 2;

        if (code_table[mid].unicode == unicodeKey)
        {
            return code_table[mid].gb;
        }
        else if (code_table[mid].unicode > unicodeKey)
        {
            end = mid - 1;
        }
        else 
        {
            first = mid + 1;
        }
    }
    return 0;
}

void Utf8ToGb2312(const char* utf8, int len, u8 *temp)
{
      
	   printf("utf8->unicode: \r\n");
       printf("utf8: [");
       for (int k = 0; k < len; k++)
       {
              printf("%02x ", utf8[k]);
       }
       printf("]\r\n");
 
       int byteCount = 0;
       int i = 0;
       int j = 0;
//	   char temp[10];
	   int k=0;

       u16 unicodeKey = 0;
       u16 gbKey = 0;


      //循环解析
       while (i < len)
       {   
        switch(GetUtf8ByteNumForWord((u8)utf8[i]))
        {
          case 0:
                temp[j] = utf8[i];
                byteCount = 1;
          break;

          case 2:
          temp[j] = utf8[i];
          temp[j + 1] = utf8[i + 1];
          byteCount = 2;
          break;

         case 3:
                 //这里就开始进行UTF8->Unicode
                 temp[j + 1] = ((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F);
                 temp[j] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);

                //取得Unicode的值
                 memcpy(&unicodeKey, (temp + j), 2);
                 printf("unicode key is: 0x%04X\r\n", unicodeKey);

                  //根据这个值查表取得对应的GB2312的值
                gbKey = SearchCodeTable(unicodeKey);
                printf("gb2312 key is: 0x%04X\r\n", gbKey);
    
                if (gbKey != 0)
                {
                       //here change the byte
                        //不为0表示搜索到，将高低两个字节调换调成我要的形式
                       gbKey = (gbKey >> 8) | (gbKey << 8);
                       printf("after changing, gb2312 key is: 0x%04X\r\n", gbKey);
                       memcpy((temp + j), &gbKey, 2);
                  }
//			gbArray[k++]=gbKey;
                byteCount = 3;
          break;

          case 4:
          byteCount = 4;
          break;
         case 5:
          byteCount = 5;
          break;
         case 6:
          byteCount = 6;
          break;
    
         default:
          printf("the len is more than 6\r\n");
          break;    
        }

        i += byteCount;
        if (byteCount == 1)
        {
               j++;
        }
        else
        {
               j += 2;
        }
  
       }
       printf("utf8: [");
       for (k = 0; k < j; k++)
       {
              printf("%02x ", temp[k]);
       }
       printf("]\r\n");
	   printf("转换后：%s\r\n",temp);
}
