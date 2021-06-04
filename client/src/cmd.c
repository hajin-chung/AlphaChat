#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cmd.h"
#include "out.h"
#include "socks.h"
#include "req.h"

void init_lobby()
{
    print_grid();
    move_cursor_cmd();
}

int check_bufecmd(char* buf, char* cmd)
{
    return (strncmp(buf, cmd, strlen(cmd)) == 0);
}

void handle_cmd()
{
    char buf[CMD_MAX_LEN];

    memset(buf, 0, CMD_MAX_LEN);
    fgets(buf, CMD_MAX_LEN, stdin); 

    cmd_log("[*] cmd %s", buf);
    move_cursor_cmd();

    if(check_bufecmd(buf, CMD_HELP))
    {
        print_help();
        print_grid();
        move_cursor_cmd();
    }
    else if(check_bufecmd(buf, CMD_ROOM_LIST)) // 
    {
        req_room_list();
    }
    else if(check_bufecmd(buf, CMD_USER_LIST))
    {
        req_user_list();
    }
    else if(check_bufecmd(buf, CMD_ROOM_CREATE))
    {
        req_room_create(buf);
    }
}
