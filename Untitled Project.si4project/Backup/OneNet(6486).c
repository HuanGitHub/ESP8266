#ifndef __ONENET_H
#define __ONENET_H
#include <string.h>
#include "OneNet.h"
#include "include.h"
#define OneNet_IP 183,230,40,39
#define OneNet_Port		876

#define Deriver_Num			29593643
struct espconn *S_OneNet_Conn;
uint8 remoteip[4]={OneNet_IP};	

void OneNet_Tcp_cline_connect();
void OneNet_Connect()
{/*{{{*/ 
	S_OneNet_Conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
	S_OneNet_Conn->type = ESPCONN_TCP;
	S_OneNet_Conn->state = ESPCONN_NONE;
	S_OneNet_Conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	S_OneNet_Conn->proto.tcp->local_port = espconn_port();
	S_OneNet_Conn->proto.tcp->remote_port = OneNet_Port;
	os_memcpy(S_OneNet_Conn->proto.tcp->remote_ip,&remoteip,4);
	//Tcp_cline_regist(S_OneNet_Conn);
	OneNet_Tcp_cline_connect();
}/*}}}*/
void OneNet_Tcp_cline_connect()
{/*{{{*/
	sint8 tcp_flag =espconn_connect(S_OneNet_Conn);
	if(0 == tcp_flag){
		os_print("OneNet Tcp Connect success");


	}else if(ESPCONN_ARG == tcp_flag){
				os_print("not find espconn cline");
			}
	
	
}/*}}}*/












#endif
