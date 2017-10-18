#ifndef __CRC16_H
#define __CRC16_H

//*puchMsg-用于计算CRC的报文，usDataLen-报文中的字节数
unsigned short CRC16_Cal(unsigned char *puchMsg, unsigned short usDataLen);

#endif
