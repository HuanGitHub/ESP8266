#include "include.h"
#include "TCP_server.h"
#include <string.h>
char *http_res = "HTTP/1.1 200 OK\r\nServer:zhanghuan\r\n" 
"Accept-Ranges:bytes\r\nContent-Length:1024\r\nConnection:close\r\n"/*{{{*/ 
"Content-Type:text/html\r\n\r\n\
<!DOCTYPE>\r\n\
<html>\r\n\
<head>\r\n\
<title>Input WIFI Info</title>\r\n\
<meta charset=unicode\" />\r\n\
</head>\r\n\
<body>\r\n\
<div id=\"content\" style=\"margin:0 auto; background-color:#0F6; width:1000px; height:500px; position:relative;\">\r\n\
		\
	<div style=\"padding-top:60px\">\r\n\
        	<p style=\"display:inline\"><center><h1>Welcome</h1></center></p>\r\n\
	</div>\r\n\
	\
<div class=\"login\" style=\"position:absolute; top:50%;left:50%;margin-top:-100px;margin-left:-200px;background:#69F; width:400px; height: 200px; border-radius:10px;\">\r\n\
	\
	<form action=\"\" method=\"post\">\
	<div style=\" margin-left:90px;margin-top:50px\">\r\n\
	    <p style=\"display:inline\">WIFIName&nbsp;&nbsp:</p>\r\n\
	    <input type=\"text\" name=\"ssid\"value=\"\"/>\r\n\
		<p style=\"display:inline\">PassWorld&nbsp;&nbsp:</p>\r\n\
		<input type=\"password\" name=\"pass\"value=\"\"/>\r\n\
   	</div>\r\n\
   	<div style=\" margin-left:350px;margin-top:35px\">\r\n\
        <input type=\"submit\" value=\"Commit\" style=\"margin:0 auto; background-color:#666 font-size:16px\"\"/>\r\n\
	</div>\r\n\
	</form>\
</div>\r\n\
</div>\r\n\
</body>\r\n\
</html>";
/*}}}*/
char get_WIFI_Set_Flag;
WIFI_Set s_WIFI_Info;
char TCP_Creat_Flag ;
char NEW_Connector = 0;
void espconn_ESP_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	if(pdata != NULL){
   		espconn_send(ESP_tcp_ser,http_res,strlen(http_res));
		os_printf("%s\n",pdata);
		os_printf("%d\r\n",strlen(http_res));
		get_WIFI_Set_Flag = get_Post_Par(pdata);		
	//	os_printf("get_WIFI_Set_flag: %d\r\n",get_WIFI_Set_Flag);

	}
}/*}}}*/
void disconnect(void *arg)
{/*{{{*/
	if(NEW_Connector != 0)
		NEW_Connector--;	
}/*}}}*/
void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/ 
	sint8 ret;
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("%s\n",data);
	if((!strcmp(data,"get") && NEW_Connector != 0 )&& (RcvData[0]!=0x00))
	{
		espconn_send(my_tcp_ser,RcvData,fifo_len);
	}
//	if(!strcmp(data,"get"))
//	{
//		os_printf("Have cline query data\r\n");
//		os_printf("Fifo_len: %d\r\nRcvData: %s\r\n",fifo_len,RcvData);
////		espconn_send(my_tcp_ser,&RcvData[0],1);
//		ret = espconn_send(my_tcp_ser,&RcvData[2],1);
//		ret = espconn_send(my_tcp_ser,&RcvData[1],1);
//	}
	ret = espconn_send(my_tcp_ser,RcvData,fifo_len);
	if(ret){
		switch(ret){
			case ESPCONN_ARG: os_printf("not find Espconn\r\n");	break;
			case ESPCONN_MEM: os_printf("MEM not enought\r\n");		break;
			case ESPCONN_MAXNUM:	os_printf("send fail\r\n");		break;
		}
	}
}/*}}}*/
void espconn_server_cb(void *arg)
{/*{{{*/
	os_printf("Have client connect\n");
	NEW_Connector ++;
}/*}}}*/
void reconnect_callback(void *arg,sint8 err)
{/*{{{*/
	switch(err)
	{
		case ESPCONN_TIMEOUT:	os_printf("TCP disconnect of Time out\r\n");	break;
		case ESPCONN_ABRT:	os_printf("TCP Exceptional disconnect\r\n");	break;
							
	}
	if(NEW_Connector != 0)
			NEW_Connector--;
	
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
		my_tcp_ser->proto.tcp->local_port = Ser_Port;
		os_printf("The Local port:%d\n",my_tcp_ser->proto.tcp->local_port);
		if(!espconn_accept(my_tcp_ser)){	
			os_printf("Begin to listen!!\n");
			espconn_regist_time(my_tcp_ser,0,0);
			TCP_Creat_Flag = 1;
		}
		Tcp_regist_fun(my_tcp_ser);
	}else{
		os_printf("Fail to listen!!\n");
			TCP_Creat_Flag = 0;
		os_timer_setfn(&ser_timer,espconn_tcp_server_creat,NULL);
		os_timer_arm(&ser_timer,1500,0);
	}
	os_free(info);
}/*}}}*/
void send_data_callback(){/*{{{*/
	os_printf("data send successful\r\n");
}/*}}}*/
void Tcp_regist_fun(struct espconn *my_tcp_ser)
{/*{{{*/
		espconn_regist_recvcb(my_tcp_ser,espconn_server_recv_cb);
		espconn_regist_connectcb(my_tcp_ser,espconn_server_cb);
		espconn_regist_disconcb(my_tcp_ser,disconnect);
		espconn_regist_reconcb(my_tcp_ser,reconnect_callback);
		espconn_regist_sentcb(my_tcp_ser,send_data_callback);
	
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
		s_WIFI_Info.ssid[len+1] = '\0';
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
	os_timer_disarm(&T_ESP_ser_timer);
	info=(struct ip_info *)os_zalloc(sizeof(struct ip_info));
	wifi_get_ip_info(SOFTAP_IF,info);

	os_printf("Begin Create ESP_TCP_Server");
	if((info->ip.addr)!=0)
	{
		ESP_tcp_ser=(struct espconn *)os_zalloc(sizeof(struct espconn));
		ESP_tcp_ser->type=ESPCONN_TCP;
		ESP_tcp_ser->state=ESPCONN_NONE;
		ESP_tcp_ser->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));

		os_memcpy(ESP_tcp_ser->proto.tcp->local_ip,info,4);
		ESP_tcp_ser->proto.tcp->local_port = ESP_Ser_Port;
		os_printf("The ESP Local port:%d\n",ESP_tcp_ser->proto.tcp->local_port);
		espconn_regist_recvcb(ESP_tcp_ser,espconn_ESP_server_recv_cb);
		espconn_regist_connectcb(ESP_tcp_ser,espconn_server_cb);
		if(!espconn_accept(ESP_tcp_ser)){	
			os_printf("Begin to listen!!\n");
			espconn_regist_time(ESP_tcp_ser,0,0);
		}
	}else{
		os_printf("Fail to listen!!\n");
		use_Timer(&T_ESP_ser_timer,espconn_ESP_tcp_server_creat,NULL,2000,0);
	}
}/*}}}*/
