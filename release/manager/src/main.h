#include <arpa/inet.h>

#define TIME_VAL_SECONDS 5
#define MULTICAST_TTL 5
#define MULTICAST_BUF_SIZE 30 

struct timeval tv; 
extern int multicast_log_flag;
extern int heartbeat_log_flag;

void multicast_server_info(int sock, struct sockaddr_in addr);
void handle_heartbeat(int sock);
void handle_new_user(int sock);

void make_mcast_socket(int* sock, struct sockaddr_in* addr, char* ip, int port);
void make_udp_socket(int* sock, int port);
void make_tcp_socket(int* sock, int port);