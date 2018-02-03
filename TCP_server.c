#include "include.h"
#include "TCP_server.h"
#include <string.h>
char *http_res = "HTTP/1.1 200 OK\r\nServer:zhanghuan\r\n"
"Accept-Ranges:bytes\r\nContent-Length:1024\r\nConnection:close\r\n"
"Content-Type:text/html\r\n\r\n\
<!DOCTYPE html>\r\n\
<html>\r\n\
<head>\r\n\
<title>Title of the document</title>\r\n\
</head>\r\n\
<body>\r\n\
<form action = \"\" method = \"post\">\r\n\
first name:\r\n\
<input type = \"text\" name = \"ssid\">\r\n\
<input type = \"text\" name = \"pass\">\r\n\
<input type = \"submit\" value = \"Submit\">\r\n\
</form>\r\n\
</body>\r\n\
</html>";
void espconn_ESP_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	if(pdata != NULL){
   		espconn_send(ESP_tcp_ser,http_res,strlen(http_res));
		os_printf("%s\n",pdata);
		get_Post_Par(pdata);		
	}
}/*}}}*/
void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("%s\n",data);
	Led_CRL(data);
	espconn_send(my_tcp_ser,http_res,strlen(http_res));
    os_free(data);	
}/*}}}*/
void espconn_server_cb(void *arg)
{/*{{{*/
	os_printf("Have client connect\n");
}/*}}}*/
void espconn_tcp_server_creat()
 {/*{{{*/
	os_timer_disarm(&ser_timer);
	info=(struct ip_info *)os_zalloc(sizeof(struct ip_info));
//	uint8 localip[4]={192,168,120,109};
	wifi_get_ip_info(SOFTAP_IF,info);
//	os_printf("ESP_IP: %u\r\n", info->ip)
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
int get_Post_Par(char *buf)
{/*{{{*/
	char *beg;
	char *end;
	char *tmp;
	int len;
	if(buf != NULL){
		beg = strstr(buf,"\r\n\r\n");
		beg += 4;
		end = strstr(beg,"=");
		if(end == NULL){
				os_printf("NOT find");
				return 0;
		}
		else{
				end += 1;
				tmp = strstr(beg ,"&");
				if(tmp != NULL)
				{
					len = tmp - end ;
				}		
		}
		if(len != 1)
			os_printf("ssid len: %d\r\n",len);
		strncpy(s_WIFI_Info.ssid, end, len);	
		beg = strstr(end ,"=");
		if(beg == NULL)
				return 0;
		beg += 1;
		strcpy(s_WIFI_Info.pass,beg);
		os_printf("end: %s\r\n WIFI_Info.ssid: %s\r\nWIFI_Info.pass %s\r\n",end,s_WIFI_Info.ssid,s_WIFI_Info.pass);
		return 1;	
	}

}/*}}}*/
void Led_CRL(char *buf)
{/*{{{*/
		
	if(strcmp(buf,"0000")==0)
	{
		os_printf("Open and close light runing");
		if((Bit12 ==1))
		{
			GPIO_12_Off();
			GPIO_13_No();
			Bit12 = 0;
			os_printf("Bit12=0\n");
		}else{		
			GPIO_12_No();
			GPIO_13_Off();
			Bit12 = 1; 
			os_printf("Bit12=1\n");
		}
	}
	if(strcmp(buf,"1111") ==0)
	{
		os_printf("change light runing");
		if(Bit14 == 1)
			{
				Bit14 = 0;
				GPIO_14_Off();
				os_printf("Bit14=0\n");
			}else{
				GPIO_14_No();
				Bit14 = 1;
				os_printf("Bit14=1\n");

			}
	}
	if(strcmp(buf,"1200")==0)
	{
		
			GPIO_12_Off();
			Bit12 = 0;
			os_printf("GPIO_12_Off");
	}
	if(strcmp(buf,"1201")==0)
	{
			GPIO_12_No();
			Bit12 = 1;
			os_printf("GPIO_12_No");
	}
	if(strcmp(buf,"1300")==0)
	{
			GPIO_13_Off();
			Bit13 = 0;
			os_printf("GPIO_13_Off");
	}
	if(strcmp(buf,"1301")==0)
	{
			GPIO_13_No();
			Bit13 = 1;
			os_printf("GPIO_13_No");
		
	}
	if(Bit13 = 0)
	{
				
			if(strcmp(buf,"1400")==0)
			{
				
					GPIO_14_Off();
					os_printf("GPIO_14_Off");
			}
				
			if(strcmp(buf,"1401")==0)
			{
					GPIO_14_No();
					os_printf("GPIO_14_No");
				
			}
	}
}/*}}}*/
void espconn_ESP_tcp_server_creat()
 {/*{{{*/
	os_timer_disarm(&ESP_ser_timer);
	info=(struct ip_info *)os_zalloc(sizeof(struct ip_info));
	wifi_get_ip_info(SOFTAP_IF,info);

	if((info->ip.addr)!=0)
	{
		os_printf("Get ip successful ip:%u\n",info->ip);
		ESP_tcp_ser=(struct espconn *)os_zalloc(sizeof(struct espconn));
		ESP_tcp_ser->type=ESPCONN_TCP;
		ESP_tcp_ser->state=ESPCONN_NONE;
		ESP_tcp_ser->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));

		os_memcpy(ESP_tcp_ser->proto.tcp->local_ip,info,4);
		ESP_tcp_ser->proto.tcp->local_port=Ser_Port;
		os_printf("The Local port:%d\n",ESP_tcp_ser->proto.tcp->local_port);
		espconn_regist_recvcb(ESP_tcp_ser,espconn_ESP_server_recv_cb);
		espconn_regist_connectcb(ESP_tcp_ser,espconn_server_cb);
		if(!espconn_accept(ESP_tcp_ser)){	
			os_printf("Begin to listen!!\n");
			espconn_regist_time(ESP_tcp_ser,0,0);
		}
	}else{
		os_printf("Fail to listen!!\n");
		os_timer_setfn(&ESP_ser_timer,espconn_tcp_server_creat,NULL);
		os_timer_arm(&ESP_ser_timer,1500,0);
	}
}/*}}}*/
