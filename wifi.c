#include "include.h"
#include "wifi.h"
#include <string.h>
void wifi_set_station_config(char *ssid,char *password)
{/*{{{*/
//	char *wifi_ssid = (char *)os_zalloc(sizeof(char));
//	char *wifi_pass = (char *)os_zalloc(sizeof(char));
//	os_memcpy(wifi_ssid,ssid,strlen(ssid));
//	os_memcpy(wifi_pass,password,strlen(password));
//	if((wifi_ssid != NULL)&& (wifi_pass != NULL)){
			struct station_config stationconf;
			wifi_set_opmode(0x03);
			stationconf.bssid_set=0;
			memset(stationconf.ssid,0,sizeof(stationconf.ssid));
			memset(stationconf.password,0,sizeof(stationconf.password));
			memcpy(stationconf.ssid,ssid,strlen(ssid));
			memcpy(stationconf.password,password,strlen(password));
			wifi_station_set_config(&stationconf);
			wifi_set_event_handler_cb(wifi_handle_event_cb);
			return ;
		
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
void wifi_esp_softap_config()
{
	wifi_set_opmode_current(0x02);	
	
}




