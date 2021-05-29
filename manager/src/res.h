//res.h : FOR RESPONSE TO CLIENT
#include "const.h"

#define RES_RESPONSE_CODE 0
#define RES_ROOM_LIST_CODE 1
#define RES_USER_LIST_CODE 2

void send_to_user(int uid, char* buf, int size);

void response_code(int uid, int code, char* log, int size);
void res_room_list(int uid);
void res_user_list(int uid);