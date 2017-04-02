#include "include.h"
void wifi_handle_event_cb(System_Event_t	*evt);
LOCAL os_timer_t cli_timer;
LOCAL os_timer_t ser_timer;
LOCAL os_timer_t recon_timer;
struct espconn *my_tcp_conn;
struct espconn *my_tcp_ser;
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{/*{{{*/
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}/*}}}*/
void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{/*{{{*/
}/*}}}*/
void user_set_station_config()
{/*{{{*/
	struct station_config stationconf;
	wifi_set_opmode(0x01);
	os_printf("\nNew wifi mode:%u\n",wifi_get_opmode());
	char ssid[32]="D402";
	char password[64]="402gk402";
	stationconf.bssid_set=0;
	memset(stationconf.ssid,0,sizeof(stationconf.ssid));
	memset(stationconf.password,0,sizeof(stationconf.password));
	memcpy(stationconf.ssid,ssid,sizeof(ssid));
	memcpy(stationconf.password,password,sizeof(password));
	wifi_station_set_config(&stationconf);
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	return ;
}/*}}}*/
void wifi_handle_event_cb(System_Event_t	*evt)
{/*{{{*/
	 os_printf("event	%x\n",	evt->event);
	 switch	(evt->event)	{
					 case	EVENT_STAMODE_CONNECTED:
					 	 os_printf("connect	to	ssid	%s,	channel	%d\n",	
	 	 	 	 	 evt->event_info.connected.ssid,	
	 	 	 	 	 evt->event_info.connected.channel);
					 	 break;
					 case	EVENT_STAMODE_DISCONNECTED:
					 	 os_printf("disconnect	from	ssid	%s,	reason	%d\n",	
	 	 	 	 	 evt->event_info.disconnected.ssid,	
	 	 	 	 	 evt->event_info.disconnected.reason);
					 	 break;
					 case	EVENT_STAMODE_AUTHMODE_CHANGE:
					 				os_printf("mode:	%d	->	%d\n",	
	 	 	 	 	 evt->event_info.auth_change.old_mode,	
	 	 	 	 	 evt->event_info.auth_change.new_mode);
					 				break;
					 case	EVENT_STAMODE_GOT_IP:
					 	 os_printf("ip:"	IPSTR 	",mask:"	IPSTR	",gw:"	IPSTR,
					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.ip), 
					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.mask),
					 	 	 	 	 		IP2STR(&evt->event_info.got_ip.gw));
					 	 os_printf("\n");
					 	 break;
					 case	EVENT_SOFTAPMODE_STACONNECTED:
					 				os_printf("station:	"	MACSTR	"join,	AID	=	%d\n",	
	 	 	 	 MAC2STR(evt->event_info.sta_connected.mac),	
	 	 	 	 evt->event_info.sta_connected.aid);
					 				break;
	 				case	EVENT_SOFTAPMODE_STADISCONNECTED:
	 								os_printf("station:	"	MACSTR	"leave,	AID	=	%d\n",	
	 	 	 	 MAC2STR(evt->event_info.sta_disconnected.mac),	
	 	 	 	 evt->event_info.sta_disconnected.aid);
					 				break;
					 default:
									os_printf("station fail");
					 	 break;
	 }
}/*}}}*/
void espconn_client_recv_cb(void *arg,char *pdata,unsigned short len)
{/*{{{*/
	char *data=(char *)os_zalloc(len+1);
	os_memcpy(data,pdata,len+1);
	os_printf("Cline recver data:%s\n",data);
 	os_free(data);
}/*}}}*/
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
void espconn_connect_cb(void *arg)
{/*{{{*/
	os_printf("TCP connect successful\n");
//	espconn_send(my_tcp_conn,"Hello",6);
}/*}}}*/
void espconn_server_cb(void *arg)
{/*{{{*/
	os_printf("Have client connect\n");
}/*}}}*/
void espconn_sent_cb(void *arg)
{/*{{{*/
	os_printf("send successful\n");
}/*}}}*/
void espconn_reconnect_cb(void *arg,sint8 err)
{/*{{{*/
	os_timer_disarm(&recon_timer);
	os_timer_setfn(&recon_timer,espconn_tcp_client_connect,NULL);
	os_timer_arm(&recon_timer,1500,0);
	os_printf("New again client Tcp server\n");
}/*}}}*/
void espconn_tcp_client_connect()
{/*{{{*/ 
	my_tcp_conn=(struct espconn *)os_zalloc(sizeof(struct espconn));
	my_tcp_conn->type=ESPCONN_TCP;
	my_tcp_conn->state=ESPCONN_NONE;
	my_tcp_conn->proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));
	my_tcp_conn->proto.tcp->local_port=espconn_port();
	my_tcp_conn->proto.tcp->remote_port=8080;
	uint8 remoteip[4]={192,168,120,102};	
	os_memcpy(my_tcp_conn->proto.tcp->remote_ip,&remoteip,4);
	espconn_regist_connectcb(my_tcp_conn,espconn_connect_cb);
	espconn_regist_reconcb(my_tcp_conn,espconn_reconnect_cb);
	espconn_regist_sentcb(my_tcp_conn,espconn_sent_cb);
	espconn_regist_recvcb(my_tcp_conn,espconn_client_recv_cb);
	espconn_connect(my_tcp_conn);
}/*}}}*/
void espconn_cli_timer()
{/*{{{*/
	struct ip_info ipconfig;
	os_timer_disarm(&cli_timer);
	wifi_get_ip_info(STATION_IF,&ipconfig);
	if(ipconfig.ip.addr){
		os_printf("Wifi connect successfun !!\n");
		os_printf("Begin connect Tcp server\n");
		espconn_tcp_client_connect();
	}else{
		os_printf("Wifi connect fail !!\n");
		os_timer_setfn(&cli_timer,espconn_cli_timer,NULL);
		os_timer_arm(&cli_timer,500,0);
	}
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
void espconn_tcp_opser_timer() 
{/*{{{*/
		os_timer_disarm(&ser_timer);
		os_timer_setfn(&ser_timer,espconn_tcp_server_creat,NULL);
		os_timer_arm(&ser_timer,1500,0);
	
}/*}}}*/
void espconn_tcp_opcli_timer()
{/*{{{*/
		os_timer_disarm(&cli_timer);
		os_timer_setfn(&cli_timer,espconn_cli_timer,NULL);
		os_timer_arm(&cli_timer,1100,0);
}/*}}}*/
void ICACHE_FLASH_ATTR
user_init(void)
{	
		uart_init(BIT_RATE_115200,BIT_RATE_115200);
		user_set_station_config();		
		espconn_tcp_opcli_timer();
		espconn_tcp_opser_timer();
}
