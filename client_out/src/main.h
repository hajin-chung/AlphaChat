#define MCAST_BUF_SIZE 30
#define TCP_PORT 3000
#define UDP_PORT 4000

#define ROOM_MAX 1000
#define ROOM_NAME_MAX_LEN 100
#define ROOM_HISTORY_LEN 5
#define ROOM_USER_MAX 10
#define ROOM_STATUS_OFF 0 
#define ROOM_STATUS_ON 1

#define USER_MAX 1000
#define USER_NAME_MAX_LEN 100
#define USER_STATUS_ONLINE 1
#define USER_STATUS_OFFLINE 0

#define LOG_MAX_LEN 100
#define MAX_REQ_BUF_SIZE 1024

// init: 
//      1. initialize file descriptor cmd_fd(stdin)
//         initialize multicast socket for recv
//      2. recieve server info from multicast socket
//         close socket
//      3. initialize sockets: HeartBeat(UDP), REQ(TCP)
void init(char* argv[]);
void handle_cmd();
void handle_res();
void send_heartbeat();