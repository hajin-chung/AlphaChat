#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "room.h"

int new_room_id()
{
    int i;

    for(i=0 ; i<ROOM_MAX ; i++)
    {
        if(rooms[i].status == ROOM_STATUS_OFF)
        {
            return i;
        }
    }

    return 0;
}

// if user is in room_id return 1
// else return 0
int room_contains_user(int room_id, int user_id)
{
    int i;
    int flag = 0;
    struct ROOM room = rooms[room_id];

    for(i=0 ; i<room.user_cnt ; i++)
    {
        if(room.users[i] == user_id)
        {
            flag = 1;
            break;
        }
    }

    return flag;
}

void push_history(int room_id, char* buf)
{
    int i;
    struct ROOM* room = &rooms[room_id];
    int hcnt = room->history_cnt;

    if(hcnt >= ROOM_HISTORY_LEN)
    {
        for(i=0 ; i<ROOM_HISTORY_LEN-1 ; i++)
        {
            memcpy(room->history[i], room->history[i+1], MAX_REQ_BUF_SIZE);
        }
    }

    if(hcnt < ROOM_HISTORY_LEN)
    {
        room->history_cnt++;
    }
    memcpy(room->history[hcnt], buf, MAX_REQ_BUF_SIZE); 
    printf("[*] Chat pushed to room %d hcnt %d\n", room_id, hcnt);
}