#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "const.h"
#include "res.h"
#include "utils.h"
#include "room.h"
#include "user.h"

// TODO error handling
void send_to_user(int uid, char* buf, int size)
{
    write(users[uid].sock, buf, size);
    printf("<~ ");
    fflush(stdout);
    write(fileno(stdout), buf, size);
}

void response_code(int uid, int cmd_code, int req_code, char* log, int size)
{
    char* buf;
    buf = malloc(size + 8);
    memset(buf, 0, size + 8);

    itoa(cmd_code, buf);
    itoa(req_code, buf+4);
    memcpy(buf+8, log, size);

    send_to_user(uid, buf, size + 8);
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
    offset = 4;
    for(i=0 ; i<users_cnt ; i++)
    {
        datatobuf(3, buf,
            INT, &users[i].id, offset, 4,
            CHAR, users[i].name, offset+4, USER_NAME_MAX_LEN,
            INT, &users[i].status, offset+4+USER_NAME_MAX_LEN, 4);
        offset += (8 + USER_NAME_MAX_LEN);
    }

    response_code(uid, REQ_USER_LIST_CODE, 200, buf, size);
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

    offset = 4;
    for(i=0 ; i<rooms_cnt; i++)
    {
        if(room_contains_user(rooms[i].id, uid))
        {
            cnt++;
            datatobuf(2, buf, 
                INT, &rooms[i].id, offset, 4,
                CHAR, rooms[i].name, offset+4, ROOM_NAME_MAX_LEN);
            offset += (ROOM_NAME_MAX_LEN+4);
        }
    }
    datatobuf(1, buf, 
        INT, &cnt, 0, 4);

    response_code(uid, REQ_ROOM_LIST_CODE, 200, buf, size);
}

void res_room_connect(int user_id, int room_id)
{
    int i;

    for(i=0 ; i<rooms[room_id].history_cnt ; i++)
    {
        response_code(user_id, REQ_ROOM_CONNECT_CODE, 200, rooms[room_id].history[i], MAX_REQ_BUF_SIZE);
    }
}