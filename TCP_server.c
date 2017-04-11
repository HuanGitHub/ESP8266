#include "include.h"
#include "TCP_server.h"

void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("Server recver data:%s\n",data);
	char *open="1";
	if(strcmp(open,data)==0)
	{
		Init_led();
		Open_led();
	}else{
		Init_led();
		Close_led();
	}
    os_free(data);	
}/*}}}*/
void espconn_server_cb(void *arg)
{/*{{{*/
	os_printf("Have client connect\n");
}/*}}}*/
void espconn_tcp_server_creat()
{/*{{{*/
	os_timer_disarm(&ser_timer);
	my_tcp_ser=(struct espconn *)os_zalloc(sizeof(struct espconn));
	my_tcp_ser->type=ESPCONN_TCP;
	my_tcp_ser->state=ESPCONN_NONE;
	my_tcp_ser->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));
	uint8 localip[4]={192,168,120,104};
	os_memcpy(my_tcp_ser->proto.tcp->local_ip,&localip,4);
	my_tcp_ser->proto.tcp->local_port=19537;
	os_printf("The Local port:%d\n",my_tcp_ser->proto.tcp->local_port);
	espconn_regist_recvcb(my_tcp_ser,espconn_server_recv_cb);
	espconn_regist_connectcb(my_tcp_ser,espconn_server_cb);
	if(!espconn_accept(my_tcp_ser))
	{
		os_printf("Begin to listen!!\n");
	}else{
		os_printf("Fail to listen!!\n");
		os_timer_setfn(&ser_timer,espconn_tcp_server_creat,NULL);
		os_timer_arm(&ser_timer,900,0);
	}
}/*}}}*/
