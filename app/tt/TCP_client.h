#define Port 8080
#define IP 192,168,120,108
LOCAL os_timer_t cli_timer;
LOCAL os_timer_t recon_timer;
struct espconn *my_tcp_conn;

void espconn_client_recv_cb(void *arg,char *pdata,unsigned short len);

void espconn_connect_cb(void *arg);

void espconn_reconnect_cb(void *arg,sint8 err);

void espconn_tcp_client_connect();

void espconn_sent_cb(void *arg);

