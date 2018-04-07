#define Ser_Port 19537
#define Ser_IP 192,168,191,109
LOCAL os_timer_t ser_timer;
struct espconn *my_tcp_ser;
struct ip_info *info;
void espconn_server_recv_cb(void *arg,char *pdata,unsigned short len);

void espconn_server_cb(void *arg);
void espconn_tcp_server_creat();


