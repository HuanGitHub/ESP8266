#include "include.h"
#include "wifi.h"
#include <string.h>
extern WIFI_Set s_WIFI_Info;
struct station_config s_Station_Config;
char WIFI_Flash_Flag ;
bool wifi_set_station_config(char *ssid,char *password)
{/*{{{*/
			struct station_config stationconf;
			char ret;
			wifi_set_opmode(0x03);
			stationconf.bssid_set=0;
			memset(stationconf.ssid,0,sizeof(stationconf.ssid));
			memset(stationconf.password,0,sizeof(stationconf.password));
			memcpy(stationconf.ssid,ssid,strlen(ssid));
			memcpy(stationconf.password,password,strlen(password));
			wifi_station_set_config(&stationconf);
		//	wifi_set_event_handler_cb(wifi_handle_event_cb);
			if(wifi_get_ip_info(STATION_IF,&ip_info))
			{
				if(ip_info.ip.addr != 0)
						ret = 1;
				else
						ret = 0;
			}
			return ret;
		
//	}
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
	
void Auto_Connect_WIFI()
{
	memset(&s_Station_Config,0,sizeof(struct station_config));
	wifi_station_get_config_default(&s_Station_Config);
	if(strlen(s_Station_Config.ssid))
	{
		wifi_set_station_config(s_Station_Config.ssid,s_Station_Config.password);
	}else{
			WIFI_Flash_Flag = 1;	
	}
		
}
void get_WIFI_state()
{/*{{{*/

			if(wifi_get_ip_info(STATION_IF,&ip_info))
			{
				if(ip_info.ip.addr != 0)
						WIFI_connect_Flag = 1;
				else
					 WIFI_connect_Flag = 0;
			}
			if(get_WIFI_Set_Flag)
			{
				if(!strlen(s_Station_Config.ssid)){
					wifi_set_station_config(s_WIFI_Info.ssid,s_WIFI_Info.pass);
					system_restart();	
				}else{
					if(strcmp(s_Station_Config.ssid,s_WIFI_Info.ssid)){
					wifi_set_station_config(s_WIFI_Info.ssid,s_WIFI_Info.pass);
					system_restart();	
						
					}
				}
					
			}
			system_out_print();
	
}/*}}}*/
void system_out_print()
{
	os_printf("\r\n--------------System_Out-------------\r\n\r\n");
	os_printf("WIFI_connect_Flag :%d\r\n",WIFI_connect_Flag);
	if(get_WIFI_Set_Flag)
					os_printf("get new WIFI SSID\r\n");

	os_printf("WIFI_Info.ssid: %s\r\nWIFI_Info.pass %s\r\n",s_WIFI_Info.ssid,s_WIFI_Info.pass);
	if(WIFI_Flash_Flag)	
			os_printf("Dont have FLASH WIFI SSID \r\n");

	os_printf("\r\n-------------------------------------\r\n\r\n");



}



