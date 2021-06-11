#include "const.h"
#include "req.h"

struct ROOM {
    int id;
    char name[ROOM_NAME_MAX_LEN];
    char history[ROOM_HISTORY_LEN][CHAT_BUF_SIZE];
    int history_cnt;
    int super_user_id;
    int users[ROOM_USER_MAX];
    int user_cnt;
    int status;
};

struct ROOM rooms[ROOM_MAX];
int rooms_cnt;

int new_room_id();
int room_contains_user(int room_id, int user_id);
void push_history(int room_id, char* buf);