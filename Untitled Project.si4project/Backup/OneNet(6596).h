#ifndef __ONENET_H
#define __ONENET_H
#include "EdpKit.h"

struct espconn *S_OneNet_Conn;



void OneNet_Connect();



EdpPacket* PacketPing(void);









#endif

