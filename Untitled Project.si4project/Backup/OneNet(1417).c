#ifndef __ONENET_H
#define __ONENET_H
#include <string.h>
#include "OneNet.h"
#include "EdpKit.h"
#include "include.h"
#include "TCP_client.h"
#include "IO.h"
#define DEBUG 1
struct espconn *S_OneNet_Conn;
extern char Cli_stat ;
#if DEBUG
#define OneNet_IP 192,168,0,101
#define OneNet_Port		8080
#else
#define OneNet_IP 183,230,40,39
#define OneNet_Port		876
#endif
sint8 ret =0;

EdpPacket Heart_pkg ;
RecvBuffer* recv_buf = NULL;

EdpPacket* pkg;
	
	
	
uint16 cmdid_len;
uint32 cmd_req_len;

char cmd_resp[] = "ok";
	
	unsigned cmd_resp_len = 0;

#define Deriver_Num			29593643

uint8 onenetip[4]={OneNet_IP};	
char* src_dev = "29593643";
char* dst_dev = "dLURLU7CrOaS=pQnfkRLVke7RBY=";

LOCAL os_timer_t T_OneNet_HeartTime;
void OneNet_Deal_CMDREQ();

void OneNet_Tcp_SendClien_Req();

void OneNet_Tcp_RecvData_Analy(char *data,unsigned short len);


void OneNet_Tcp_Init_EdpPack();

void OneNet_Tcp_cline_connect();
void Espconn_Error(char* ch,sint8 ret);
void OneNet_Tcp_Cline_HeartBeat();

void OneNet_Tcp_send_cb(void *arg)
{/*{{{*/
//	os_printf("OneNet Tcp send successful\n");
}/*}}}*/
void OneNet_Tcp_ClineOff_cb()
{/*{{{*/
	if(Cli_stat==1)
		Cli_stat = 0;
	os_printf("cline off\r");
	
}/*}}}*/
void OneNet_Tcp_Cline_ReData_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/
	int i;
	
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("Rvce Data form Service:\r\n");
	for(i=0;i<len;i++){
		os_printf("%x\t",*data++);
	}
//	os_free(data);
	OneNet_Tcp_RecvData_Analy(pdata,len);

	
	//os_printf("Rvce Data form Service: %s\n",data);
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
	

}




void OneNet_Tcp_SendClien_Req()
{
	int i;
	uint8 *str = (uint8 *)os_zalloc(60);
	EdpPacket* pkg = NULL;
	 pkg = PacketConnect1(src_dev, dst_dev);

	 os_memcpy(str,pkg->_data,pkg->_write_pos);   //将得到的pkg包data cpy到uint8* xx 中发送 ？
	 os_printf("pkg_data:\r\n");
	for(i=0;i<pkg->_write_pos;i++){
	 os_printf("%x\t",*str++);
	}
	os_printf("\r\n");
	os_free(str);
	
	ret = espconn_send(S_OneNet_Conn,pkg->_data,pkg->_write_pos);

//	os_printf("_write_pos:%d\r\n ",pkg->_write_pos);
	Espconn_Error("OneNet Cline req",ret);
	DeleteBuffer(&pkg);
	
	OneNet_Tcp_Init_EdpPack();

	use_Timer(&T_OneNet_HeartTime,OneNet_Tcp_Cline_HeartBeat,NULL,5000,1);
}

void OneNet_Tcp_RecvData_Analy(char *data,unsigned short len)
{
	uint8 mtype;
	
//	os_printf("Begin anayl data");

	WriteBytes(recv_buf, data, len);
	 if ((pkg = GetEdpPacket(recv_buf)) == 0)
     {
         os_printf("need more bytes...\n");        
     }
	 
	  mtype = EdpPacketType(pkg);

	  switch (mtype)
	 {
		  case CMDREQ: 		os_printf("recv data type: CMDREQ");
							OneNet_Deal_CMDREQ();
               				break;
		  case PINGRESP:	break; 
			default:	os_printf("Unparsed type\r\n");break;
	 }
	DeleteBuffer(&pkg);
	//  os_printf("fainsh anayl data");

}

/*
	处理CMDREQ请求函数

*/

void OneNet_Deal_CMDREQ()
{
	EdpPacket* send_pkg;
	char*	cmdid;
	char*  	cmd_req;
	if (UnpackCmdReq(pkg, &cmdid, &cmdid_len,
					&cmd_req, &cmd_req_len) == 0){
						/*
						 * 用户按照自己的需求处理并返回，响应消息体可以为空，此处假设返回2个字符"ok"。
						 * 处理完后需要释放
						 */
						if(cmd_req == 0x31){
							GPIO_12_No();
							os_printf("Recv Data 0x31");
						}else if(cmd_req == 0x30){
							GPIO_12_Off();
							os_printf("Recv Data 0x30");
						}
						
						os_printf("\r\nOneNet req data:%x\r\n",*cmd_req);
						cmd_resp_len = os_strlen(cmd_resp);
						send_pkg = PacketCmdResp(cmdid, cmdid_len,
												 cmd_resp, cmd_resp_len);
	
						ret = espconn_send(S_OneNet_Conn,send_pkg->_data,send_pkg->_write_pos);
	
					//	os_printf("_write_pos:%d\r\n ",send_pkg->_write_pos);
						Espconn_Error("Send OK",ret);
					
						DeleteBuffer(&send_pkg);
				
						os_free(cmdid);
						os_free(cmd_req);
					}


}

void OneNet_Tcp_Init_EdpPack()
{
	recv_buf = NewBuffer();
	Init_led();
	os_memcpy(&Heart_pkg,PacketPing(),sizeof(EdpPacket));
//	DeleteBuffer(&t_pkg);
}
void OneNet_Tcp_Cline_HeartBeat()
{
//	EdpPacket* Heart_pkg = NULL;
//	Heart_pkg = PacketPing();
	ret = espconn_send(S_OneNet_Conn,Heart_pkg._data,2);

//	os_printf("_write_pos:%d\r\n ",Heart_pkg->_write_pos);
	Espconn_Error("OneNet HeartBeat",ret);
//	DeleteBuffer(&Heart_pkg);



}







#endif
