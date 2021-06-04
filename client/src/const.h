// const.h : shared constants

// server info
#define MULTICAST_IP "239.0.1.100"
#define MULTICAST_PORT 5000

// room specific
#define ROOM_NAME_MAX_LEN 100
#define ROOM_HISTORY_LEN 5
#define ROOM_STATUS_OFF 0 
#define ROOM_STATUS_ON 1

// user specific
#define USER_NAME_MAX_LEN 100
#define USER_STATUS_ONLINE 1
#define USER_STATUS_OFFLINE 0

// res specific
#define LOG_MAX_LEN 100
#define MAX_REQ_BUF_SIZE 1024

#define REQ_ROOM_CREATE_CODE 8
#define REQ_ROOM_DELETE_CODE 1 
#define REQ_ROOM_CONNECT_CODE 2 
#define REQ_SEND_CHAT_CODE 3
#define REQ_ROOM_INVITE_CODE 4
#define REQ_REGISTER_CODE 5
#define REQ_ROOM_LIST_CODE 6
#define REQ_USER_LIST_CODE 7
#define REQ_FILE_UPLOAD_CONTENT_CODE 10
#define REQ_FILE_DOWNLOAD 9 
