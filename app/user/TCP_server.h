#include "espconn.h"
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#define Ser_Port 19536
#define ESP_Ser_Port 80
LOCAL os_timer_t ser_timer;
LOCAL os_timer_t T_ESP_ser_timer;
struct espconn *my_tcp_ser;
struct espconn *ESP_tcp_ser;
struct ip_info *info;
void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len);
extern char get_WIFI_Set_Flag;
void espconn_server_cb(void *arg);
void espconn_tcp_server_creat();
void Tcp_regist_fun(struct espconn *my_tcp_ser);
typedef struct{
	char ssid[20];
	char pass[20];	
}WIFI_Set;

int get_Post_Par(char *buf);
void espconn_ESP_tcp_server_creat();
void Led_CRL(char *buf);
void espconn_ESP_server_recv_cb(void *arg,char *pdata,unsigned short len);

#endif

