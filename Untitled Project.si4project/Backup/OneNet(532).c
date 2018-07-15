#ifndef __ONENET_H
#define __ONENET_H
#include <string.h>
#include "OneNet.h"
#include "EdpKit.h"
#include "include.h"
#include "TCP_client.h"
struct espconn *S_OneNet_Conn;

#define OneNet_IP 183,230,40,39
#define OneNet_Port		876

//#define OneNet_IP 192,168,0,101
//#define OneNet_Port		8080



#define Deriver_Num			29593643

uint8 onenetip[4]={OneNet_IP};	
char* src_dev = "29593643";
char* dst_dev = "dLURLU7CrOaS=pQnfkRLVke7RBY=";

void OneNet_Tcp_cline_connect();
void OneNet_Connect()
{/*{{{*/ 

	EdpPacket* send_pkg;
	sint8 ret =0;
	sint8 tcp_flag;

	S_OneNet_Conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
	S_OneNet_Conn->type = ESPCONN_TCP;
	S_OneNet_Conn->state = ESPCONN_NONE;
	S_OneNet_Conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	S_OneNet_Conn->proto.tcp->local_port = espconn_port();
	S_OneNet_Conn->proto.tcp->remote_port = OneNet_Port;
	os_memcpy(S_OneNet_Conn->proto.tcp->remote_ip,&onenetip,4);
	Tcp_cline_regist(S_OneNet_Conn);

	
	tcp_flag =espconn_connect(S_OneNet_Conn);
	
	if(0 == tcp_flag){
		os_printf("OneNet Tcp Connect success\r\n");
		
		send_pkg = PacketConnect1(src_dev,dst_dev);

		ret = espconn_send(S_OneNet_Conn,(unsigned char*)send_pkg->_data,sizeof(send_pkg->_data));

		os_printf("send_data: %u \r\n",send_pkg->_data);
		if(ret == 0){
			os_printf("OneNet send success\r\n send_data:");
		}else{
			if(ret == ESPCONN_ARG)
				os_printf("not find conn par");
			if(ret == ESPCONN_MEM)
				os_printf("mem not engof");
			if(ret == ESPCONN_MAXNUM)
				os_printf("foo send fail");
			
			os_printf("ret: %d\r\n",ret);
		}
	
	}else if(ESPCONN_ARG == tcp_flag){
				os_printf("not find espconn cline");
			}
//	OneNet_Tcp_cline_connect();
}/*}}}*/
void OneNet_Tcp_cline_connect()
{/*{{{*/
	EdpPacket* send_pkg;
	sint8 ret =0;
	sint8 tcp_flag =espconn_connect(S_OneNet_Conn);
	
	if(0 == tcp_flag){
		os_printf("OneNet Tcp Connect success\r\n");
		
		send_pkg = PacketConnect1(src_dev,dst_dev);

		ret = espconn_send(S_OneNet_Conn,(unsigned char*)send_pkg->_data,sizeof(send_pkg->_data));

		//os_printf("send_data: %s \r\n",send_pkg->_data);
		if(ret == 0){
			os_printf("OneNet send success\r\n send_data:");
		}else{
			if(ret == ESPCONN_ARG)
				os_printf("not find conn par");
			if(ret == ESPCONN_MEM)
				os_printf("mem not engof");
			if(ret == ESPCONN_MAXNUM)
				os_printf("foo send fail");
			
			os_printf("ret: %d\r\n",ret);
		}
	
	}else if(ESPCONN_ARG == tcp_flag){
				os_printf("not find espconn cline");
			}
}/*}}}*/





EdpPacket* PacketPing(void)
{
    EdpPacket* pkg = NULL;

    pkg = NewBuffer();
    /* msg type */
    WriteByte(pkg, PINGREQ);
    /* remain len */
    WriteRemainlen(pkg, 0);
    return pkg;
}










#endif
