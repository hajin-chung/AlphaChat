#include "user.h"

int new_user_id()
{
    int i;

    for(i=0 ; i<USER_MAX; i++)
    {
        if(users[i].status == USER_STATUS_OFFLINE)
        {
            return i;
        }
    }

    return 0;
}