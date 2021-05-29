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