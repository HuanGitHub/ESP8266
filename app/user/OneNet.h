#ifndef __ONENET_H
#define __ONENET_H
#include "EdpKit.h"
#include "include.h"
struct espconn *S_OneNet_Conn;

LOCAL os_timer_t T_OneNet_HeartTime;
void OneNet_Tcp_Cline_HeartBeat();


void OneNet_Connect_TcpSer();



void OneNet_Tcp_SendClien_Req();



void Espconn_Error(char* ch,sint8 ret);




void OneNet_Tcp_RecvData_Analy(char *data,unsigned short len);




#endif

