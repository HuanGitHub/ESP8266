#ifndef __WIFI_H
#define __WIFI_H
//bool wifi_set_station_config(char ssid1[32],char password[64]);
struct ip_info ip_info;

extern char WIFI_Flash_Flag ;
char WIFI_connect_Flag;
LOCAL os_timer_t T_get_WIFI;
bool wifi_set_station_config(char *ssid,char *password);
void wifi_handle_event_cb(System_Event_t *evt);

void get_WIFI_state();
void system_out_print();

void Auto_Connect_WIFI();
#endif
