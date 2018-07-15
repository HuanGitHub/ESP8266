#ifndef __ONENET_H
#define __ONENET_H
#include <string.h>
#include "OneNet.h"
#include "EdpKit.h"
#include "include.h"
#include "TCP_client.h"
struct espconn *S_OneNet_Conn;
extern char Cli_stat ;

#define OneNet_IP 183,230,40,39
#define OneNet_Port		876

//#define OneNet_IP 192,168,0,101
//#define OneNet_Port		8080

sint8 ret =0;


#define Deriver_Num			29593643

uint8 onenetip[4]={OneNet_IP};	
char* src_dev = "29593643";
char* dst_dev = "dLURLU7CrOaS=pQnfkRLVke7RBY=";
void OneNet_Tcp_SendClien_Req();

void OneNet_Tcp_cline_connect();
void Espconn_Error(char* ch,sint8 ret);

void OneNet_Tcp_send_cb(void *arg)
{/*{{{*/
	os_printf("OneNet Tcp send successful\n");
}/*}}}*/
void OneNet_Tcp_ClineOff_cb()
{/*{{{*/
	if(Cli_stat==1)
		Cli_stat = 0;
	os_printf("cline off\r");
	
}/*}}}*/
void OneNet_Tcp_Cline_ReData_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("Rvce Data form Service: %s\n",data);
}/*}}}*/
void OneNet_Tcp_ConnectTure_cb(void *arg)
{/*{{{*/
	Cli_stat = 1;
	os_printf("TCP connect successful\n");
	OneNet_Tcp_SendClien_Req();

//	espconn_send(my_tcp_conn,"Hello",6);
}/*}}}*/
void OneNet_Tcp_ReConnect_cb(void *arg,sint8 err)
{/*{{{*/
	os_timer_disarm(&recon_timer);
	os_timer_setfn(&recon_timer,OneNet_Tcp_cline_connect,NULL);
	os_timer_arm(&recon_timer,1500,0);
	os_printf("New again client Tcp server IP:%u,%u,%u,%u:%ld\n",onenetip[0],onenetip[1],onenetip[2],onenetip[3],OneNet_Port);
}/*}}}*/

void OneNet_Tcp_Cline_Regist(struct espconn *my_tcp_conn)
{/*{{{*/
	espconn_regist_connectcb(my_tcp_conn,OneNet_Tcp_ConnectTure_cb);

	espconn_regist_reconcb(my_tcp_conn,OneNet_Tcp_ReConnect_cb);

	espconn_regist_sentcb(my_tcp_conn,OneNet_Tcp_send_cb);

	espconn_regist_recvcb(my_tcp_conn,OneNet_Tcp_Cline_ReData_cb);

	espconn_regist_disconcb(my_tcp_conn,OneNet_Tcp_ClineOff_cb);
	
}/*}}}*/

void OneNet_Connect_TcpSer()
{/*{{{*/ 

	/*	init espconn */
	S_OneNet_Conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
	S_OneNet_Conn->type = ESPCONN_TCP;
	S_OneNet_Conn->state = ESPCONN_NONE;
	S_OneNet_Conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	S_OneNet_Conn->proto.tcp->local_port = espconn_port();
	S_OneNet_Conn->proto.tcp->remote_port = OneNet_Port;
	os_memcpy(S_OneNet_Conn->proto.tcp->remote_ip,&onenetip,4);

	OneNet_Tcp_Cline_Regist(S_OneNet_Conn);           //注册各种回调函数
		
	
	
	OneNet_Tcp_cline_connect();
	
		
//	OneNet_Tcp_cline_connect();
}/*}}}*/
void OneNet_Tcp_cline_connect()
{/*{{{*/
	os_printf("begin Cline tcp server\r\n");
	ret =espconn_connect(S_OneNet_Conn);

	Espconn_Error("S_OneNet_Conn",ret);
	os_printf("finsh Cline tcp server\r\n");

}/*}}}*/
 
void Espconn_Error(char* ch,sint8 ret)
{
	if(ret == 0){
		os_printf("%s success\r\n ",ch);
	}else{
				if(ret == ESPCONN_ARG)
					os_printf("%s not find conn par\r\n",ch);
				if(ret == ESPCONN_MEM)
					os_printf("%s mem not enough\r\n",ch);
				if(ret == ESPCONN_MAXNUM)
					os_printf("%s send fail\r\n",ch);
				
				os_printf("ret: %d\r\n",ret);
	}
	os_printf("ret: %d\r\n",ret);

}



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
void OneNet_Tcp_SendClien_Req()
{
	int i;
	 EdpPacket* pkg = NULL;
	 pkg = PacketConnect1(src_dev, dst_dev);
	for(i=0;i<pkg->_write_pos;i++){
	 os_printf("pkg_data: %x",pkg->_data++);
		}

}










#endif
