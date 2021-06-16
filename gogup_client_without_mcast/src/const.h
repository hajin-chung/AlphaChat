#ifndef CONST_H
#define CONST_H

#define CMD_CNT 13
#define CMD_ROOM_CREATE_CODE 8
#define CMD_ROOM_DELETE_CODE 1 
#define CMD_ROOM_CONNECT_CODE 2 
#define CMD_SEND_CHAT_CODE 3
#define CMD_ROOM_INVITE_CODE 4
#define CMD_REGISTER_CODE 5
#define CMD_ROOM_LIST_CODE 6
#define CMD_USER_LIST_CODE 7
#define CMD_FILE_UPLOAD_CONTENT_CODE 10
#define CMD_FILE_DOWNLOAD 9 
#define CMD_TOGGLE_HB_LOG 11
#define CMD_HELP 12
#define CMD_EXIT 13

#define CMD_STATUS_SUCCESS 200
#define CMD_STATUS_FAIL 500

#define CONTENTS_MAX_LEN 512

#define CHAT_TYPE_TEXT 1
#define CHAT_TYPE_EMOJ 2
#define CHAT_TYPE_FILE 3

#define CMD_MAX_LEN 1024

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

extern char server_ip[30];
extern int server_port;

// USER
extern int my_user_id;
extern char my_user_name[USER_NAME_MAX_LEN];

// ROOM
extern int cur_room_id;
extern int cur_room_name[ROOM_NAME_MAX_LEN];

extern int heartbeat_log_flag;

#endif