//req.h : FOR REQUEST FROM CLIENT

#include "const.h"

#define ROOMPASS_MAX_LEN 20
#define CONTENTS_MAX_LEN 512

#define REQ_ROOM_CREATE_CODE 8
#define REQ_ROOM_DELETE_CODE 1 
#define REQ_ROOM_CONNECT_CODE 2 
#define REQ_SEND_CHAT_CODE 3
#define REQ_ROOM_INVITE_CODE 4
#define REQ_REGISTER_CODE 5
#define REQ_ROOM_LIST_CODE 6
#define REQ_USER_LIST_CODE 7

#define CHAT_TYPE_TEXT 1
#define CHAT_TYPE_EMOJ 2
#define CHAT_TYPE_FILE 3

struct SendChat {
	int type;
	int user_id;
	int room_id;
	char contents[CONTENTS_MAX_LEN];
};

void user_register(char* buf, int user_id);
void room_create(char* buf, int user_id);
void room_delete(char* buf, int user_id);
void room_connect(char* buf, int user_id);
void room_invite(char* buf, int user_id);
void send_chat(char* buf, int user_id);
void user_list(char* buf, int user_id);
void room_list(char* buf, int user_id);

void handle_request(int sock, int user_id);