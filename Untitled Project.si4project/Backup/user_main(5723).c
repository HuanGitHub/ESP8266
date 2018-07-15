#include "include.h"
#define TCP_SERVER 	0
#define TCP_CLINE	0
#define ONENET_TCP  1
extern WIFI_Set s_WIFI_Info;
LOCAL os_timer_t TCP_timer;
LOCAL os_timer_t SData_timer;
extern char Cli_stat;
extern struct espconn *my_tcp_conn;
uint8 NULL_Buff[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00a};
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
void espconn_cli_timer()
{/*{{{*/
//	struct ip_info ipconfig;
	os_timer_disarm(&cli_timer);
//	wifi_get_ip_info(STATION_IF,&ipconfig);
//	if(ipconfig.ip.addr){
		os_printf("Wifi connect successfun !!\n");
//		os_printf("Begin connect Tcp server\n");
//	}else{
//		os_printf("Wifi connect fail !!\n");
//		os_timer_setfn(&cli_timer,espconn_cli_timer,NULL);
//		os_timer_arm(&cli_timer,500,0);
//	}
}/*}}}*/
void espconn_tcp_opser_timer() 
{ /*{{{*/
		if(WIFI_connect_Flag == 1)
		{
//		 	os_printf("tcp_open/r/n");
			os_timer_disarm(&ser_timer);
			os_timer_setfn(&ser_timer,espconn_tcp_server_creat,NULL);
			os_timer_arm(&ser_timer,1500,0);
		}
	
}/*}}}*/
void espconn_ESP_tcp_opser_timer() 
{/*{{{*/
	wifi_set_opmode_current(0x02);	
		os_timer_disarm(&T_ESP_ser_timer);
		os_timer_setfn(&T_ESP_ser_timer,espconn_ESP_tcp_server_creat,NULL);
		os_timer_arm(&T_ESP_ser_timer,1500,0);
	
}/*}}}*/
void use_Timer(os_timer_t *timer,os_timer_func_t *pfunction,void *parg,int t,char re_falg)
{/*{{{*/
	os_timer_disarm(timer);
	os_timer_setfn(timer,pfunction,parg);
	os_timer_arm(timer,t,re_falg);
}/*}}}*/
void os_init()
{/*{{{*/
	uart_init(BIT_RATE_115200,BIT_RATE_115200);
	wifi_set_opmode_current(0x02);	
// 	os_memset(s_WIFI_Info,0,sizeof(WIFI_Set));
//	os_memset(tt,0,sizeof(st)); //?
	WIFI_Flash_Flag = 0;
	Auto_Connect_WIFI();
	os_printf("OS_INIT over");
	
}/*}}}*/
void espconn_tcp_opcli_timer()
{/*{{{*/
		os_timer_disarm(&cli_timer);
//		os_timer_setfn(&cli_timer,espconn_cli_timer,NULL);
//		os_timer_arm(&cli_timer,1100,0);
		espconn_tcp_client_connect();
}/*}}}*/
void Begin_TCP()
{/*{{{*/
    if(WIFI_connect_Flag)
	{
		os_timer_disarm(&TCP_timer);	
#if TCP_SERVER
		use_Timer(&ser_timer,espconn_tcp_opser_timer,NULL,4000,1);
#endif
#if TCP_CLINE
	//	use_Timer(&cli_timer,espconn_tcp_client_connect,NULL,4000,1);
		espconn_tcp_client_connect();
#endif
#if ONENET_TCP
		OneNet_Connect();
#endif
	}else{
		os_printf("WIFI not connect\n");	
	}
	
}/*}}}*/
void Send_Data()
{/*{{{*/
	sint8 ret =0;EdpPacket* send_pkg;
	char* src_dev = "29593643";
char* dst_dev = "dLURLU7CrOaS=pQnfkRLVke7RBY=";
	if(Cli_stat)
	{
		send_pkg = PacketConnect1(src_dev,dst_dev);
		ret = espconn_send(S_OneNet_Conn,(unsigned char*)send_pkg->_data,sizeof(send_pkg->_data));
		

		if(ret == 0){
			os_printf("OneNet send success\r\n send_data:");
		}else{
			if(ret == ESPCONN_ARG)
				os_printf("not find conn par");
			if(ret == ESPCONN_MEM)
				os_printf("mem not engof");
			if(ret == ESPCONN_MAXNUM)
				os_printf("foo send fail");
			}

			
	}
}/*}}}*/
user_init(void)
{	
		os_init();
//		wifi_set_station_config("iPhone","123456789");
		use_Timer(&T_get_WIFI,get_WIFI_state,NULL,2000,1);		//get WIFI state
		use_Timer(&T_ESP_ser_timer,espconn_ESP_tcp_server_creat,NULL,2000,0);		//Begin ESP_TCP_server
		use_Timer(&TCP_timer,Begin_TCP,NULL,1000,1);
		use_Timer(&SData_timer,Send_Data,NULL,1000,1);
	

}		 

