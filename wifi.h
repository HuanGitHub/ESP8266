#ifndef __WIFI_H
#define __WIFI_H

void wifi_set_station_config(char ssid1[32],char password[64]);
void wifi_handle_event_cb(System_Event_t *evt);

void wifi_esp_softap_config();


#endif
