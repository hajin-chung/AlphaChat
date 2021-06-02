// const.h : shared constants

// server info
#define MULTICAST_IP "239.0.1.100"
#define MULTICAST_PORT 5000
#define SERVER_IP "172.27.70.195"
#define TCP_PORT 3000
#define UDP_PORT 4000

// room specific
#define ROOM_MAX 1000
#define ROOM_NAME_MAX_LEN 100
#define ROOM_HISTORY_LEN 5
#define ROOM_USER_MAX 10
#define ROOM_STATUS_OFF 0 
#define ROOM_STATUS_ON 1

// user specific
#define USER_MAX 1000
#define USER_NAME_MAX_LEN 100
#define USER_STATUS_ONLINE 1
#define USER_STATUS_OFFLINE 0

// res specific
#define LOG_MAX_LEN 100
#define MAX_REQ_BUF_SIZE 1024