#include "include.h"
#include "TCP_client.h"
#define Port 8080
#define IP 192,168,0,107
uint8 remoteip[4]={IP};	
char Cli_stat =0;
void Tcp_cline_regist(struct espconn *my_tcp_conn);
void Tcp_cline_connect();
void cli_disconnect();
void espconn_client_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("From Server data:%s\n",data);
}/*}}}*/
void espconn_connect_cb(void *arg)
{/*{{{*/
	Cli_stat = 1;
	os_printf("TCP connect successful\n");
	espconn_send(my_tcp_conn,"Hello",6);
}/*}}}*/
void espconn_reconnect_cb(void *arg,sint8 err)
{/*{{{*/
	os_timer_disarm(&recon_timer);
	os_timer_setfn(&recon_timer,Tcp_cline_connect,NULL);
	os_timer_arm(&recon_timer,1500,0);
	os_printf("New again client Tcp server IP:%u,%u,%u,%u:%ld\n",remoteip[0],remoteip[1],remoteip[2],remoteip[3],Port);
}/*}}}*/
void espconn_tcp_client_connect()
{/*{{{*/ 
	my_tcp_conn = (struct espconn *)os_zalloc(sizeof(struct espconn));
	my_tcp_conn->type = ESPCONN_TCP;
	my_tcp_conn->state = ESPCONN_NONE;
	my_tcp_conn->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	my_tcp_conn->proto.tcp->local_port = espconn_port();
	my_tcp_conn->proto.tcp->remote_port = Port;
	os_memcpy(my_tcp_conn->proto.tcp->remote_ip,&remoteip,4);
	Tcp_cline_regist(my_tcp_conn);
	Tcp_cline_connect();
}/*}}}*/
void Tcp_cline_connect()
{/*{{{*/
	espconn_connect(my_tcp_conn);
}/*}}}*/
void espconn_sent_cb(void *arg)
{/*{{{*/
	os_printf("send successful\n");
}/*}}}*/
void Tcp_cline_regist(struct espconn *my_tcp_conn)
{/*{{{*/
	espconn_regist_connectcb(my_tcp_conn,espconn_connect_cb);

	espconn_regist_reconcb(my_tcp_conn,espconn_reconnect_cb);

	espconn_regist_sentcb(my_tcp_conn,espconn_sent_cb);

	espconn_regist_recvcb(my_tcp_conn,espconn_client_recv_cb);

	espconn_regist_disconcb(my_tcp_conn,cli_disconnect);
	
}/*}}}*/
void cli_disconnect()
{/*{{{*/
	if(Cli_stat==1)
		Cli_stat = 0;
	os_printf("cline off\r");
	
}/*}}}*/
