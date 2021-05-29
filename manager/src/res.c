#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "res.h"
#include "utils.h"
#include "room.h"
#include "user.h"

// TODO error handling
void send_to_user(int uid, char* buf, int size)
{
    struct USER user = users[uid];

    write(user.sock, buf, size);
}

void response_code(int uid, int code, char* log, int size)
{
    char buf[104];
    memset(buf, 0, 104);

    itoa(code, buf);
    memcpy(buf, log+4, size);

    send_to_user(uid, buf, 104);
}

void res_user_list(int uid)
{
    int i;
    char* buf;
    int size = 4 + 108 * users_cnt;
    int offset = 0;

    buf = malloc(size);
    memset(buf, 0, size);

    itoa(users_cnt, buf); 
    offset += 4;
    for(i=0 ; i<users_cnt ; i++)
    {
        itoa(users[i].id, buf + offset);
        offset += 4;
        memcpy(buf + offset, users[i].name, USER_NAME_MAX_LEN);
        offset += USER_NAME_MAX_LEN;
        itoa(users[i].status, buf + offset);
        offset += 4;
    }

    printf("[*] Res user list size(%d) user_cnt(%d): ", size, users_cnt);
    fwrite(buf, sizeof(char), size, stdout);
    printf("\n");
    send_to_user(uid, buf, size);
}

void res_room_list(int uid)
{
    int i;
    char* buf;
    int size = 4 + 104 * rooms_cnt;
    int offset = 0;
    int cnt = 0;

    buf = malloc(size);
    memset(buf, 0, size);

    offset += 4;
    for(i=0 ; i<rooms_cnt; i++)
    {
        if(room_contains_user(rooms[i].id, uid))
        {
            cnt++;
            itoa(rooms[i].id, buf + offset);
            offset += 4;
            memcpy(buf + offset, rooms[i].name, ROOM_NAME_MAX_LEN);
            offset += ROOM_NAME_MAX_LEN;
        }
    }
    itoa(cnt, buf);

    printf("[*] Res room list: ");
    fwrite(buf, sizeof(char), size, stdout);
    printf("\n");
    send_to_user(uid, buf, size);
}