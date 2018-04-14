#include "include.h"
#include "TCP_server.h"
#include "espconn.h"
void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	sint8 ret;
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("%s\n",data);
//	espconn_send(my_tcp_ser,RcvData,fifo_len);
	os_printf("fifo_len:%d\r\nRcvData:%s\r\n",fifo_len,RcvData);
	if(RcvData[0]=='\0')
	{
		os_printf("no have data\r\n");
		ret = espconn_send(my_tcp_ser,"no have data\n",15);
		switch(ret)
		{
			case ESPCONN_ARG: os_printf("not find tcp cline\r\n"); break;
			case ESPCONN_MEM: os_printf("not have enought Ram\r\n");	break;
			case  ESPCONN_MAXNUM:	os_printf("send fail\r\n");	break;

		}
	}else{
//		espconn_send(my_tcp_ser,RcvData,fifo_len);
		ret = espconn_send(my_tcp_ser,RcvData,fifo_len);
		switch(ret)
		{
			case ESPCONN_ARG: os_printf("not find tcp cline\r\n"); break;
			case ESPCONN_MEM: os_printf("not have enought Ram\r\n");	break;
			case  ESPCONN_MAXNUM:	os_printf("send fail\r\n");	break;

		}
		os_printf("send to cline\r\n");
		}
//	os_free(data);	
}/*}}}*/
void espconn_server_cb(void *arg)
{/*{{{*/
	os_printf("Have client connect\n");
}/*}}}*/
void espconn_tcp_server_creat()
{/*{{{*/
	os_timer_disarm(&ser_timer);
	info=(struct ip_info *)os_zalloc(sizeof(struct ip_info));
	wifi_get_ip_info(STATION_IF,info);
	if((info->ip.addr)!=0)
	{
		os_printf("Get ip successful ip:%u\n",info->ip);
		my_tcp_ser=(struct espconn *)os_zalloc(sizeof(struct espconn));
		my_tcp_ser->type=ESPCONN_TCP;
		my_tcp_ser->state=ESPCONN_NONE;
		my_tcp_ser->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));
		os_memcpy(my_tcp_ser->proto.tcp->local_ip,info,4);
		my_tcp_ser->proto.tcp->local_port=Ser_Port;
		os_printf("The Local port:%d\n",my_tcp_ser->proto.tcp->local_port);
		espconn_regist_recvcb(my_tcp_ser,espconn_server_recv_cb);
		espconn_regist_connectcb(my_tcp_ser,espconn_server_cb);
		if(!espconn_accept(my_tcp_ser)){	
			os_printf("Begin to listen!!\n");
			espconn_regist_time(my_tcp_ser,0,0);
		}
	}else{
		os_printf("Fail to listen!!\n");
		os_timer_setfn(&ser_timer,espconn_tcp_server_creat,NULL);
		os_timer_arm(&ser_timer,1500,0);
	}
}/*}}}*/
