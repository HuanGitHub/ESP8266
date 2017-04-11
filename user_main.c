#include "include.h"
#include "wifi.h"
#include "TCP_server.h"
#include "TCP_client.h"
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
		wifi_set_station_config("D402","402gk402");		
	
	//	wifi_set_station_config("iPhone","123456789");		
		espconn_tcp_opcli_timer();
		espconn_tcp_opser_timer();
	//	Open_len();
}
