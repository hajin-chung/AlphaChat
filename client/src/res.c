#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "const.h"
#include "res.h"
#include "cmd.h"
#include "utils.h"
#include "out.h"

void handle_res(int sock)
{
    char buf[MAX_REQ_BUF_SIZE];
    int res_code;
    int req_code;
    long len;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    len = read(sock, buf, MAX_REQ_BUF_SIZE);
    if(len <= 0)
    {
        return;
    }

    buftodata(2, buf, 
        INT, &req_code, 0, 4,
        INT, &res_code, 4, 4);

    if(req_code == REQ_REGISTER_CODE)
    {
        splash_screen_log("[*] Register response %d %d", req_code, res_code);
        user_register(buf, res_code);
    }
    if(req_code == REQ_ROOM_LIST_CODE)
    {
        res_room_list(buf, res_code);
    }
    if(req_code == REQ_USER_LIST_CODE)
    {
        res_user_list(buf, res_code);
    }
}

void user_register(char* buf, int code)
{
    if(code == 200) 
    {
        init_lobby();
    }
    else if(code == 500)
    {
        printf("[!] User register error!");
        exit(0);
    }
    else
    {
        printf("\n\n[!] Invalid response code %d", code);
        exit(0);
    }
}

void res_room_list(char *buf, int code)
{
    int i, offset;
    int room_cnt;
    int room_id;
    char room_name[ROOM_NAME_MAX_LEN];
    char out[ROOM_NAME_MAX_LEN + 4];

    if(code == 200)
    {
        print_to_lobby("[*] Room list\n");
        buftodata(1, buf, INT, &room_cnt, 8, 4);        
        offset = 12;
        for(i=0 ; i<room_cnt ; i++)
        {
            memset(room_name, 0, ROOM_NAME_MAX_LEN);
            memset(out, 0, ROOM_NAME_MAX_LEN + 4);

            buftodata(2, buf, 
                INT, &room_id, offset, 4,
                CHAR, room_name, offset+4, ROOM_NAME_MAX_LEN);
            offset += (4 + ROOM_NAME_MAX_LEN);
            cmd_log("%s [%d]", room_name, room_id);

            sprintf(out, "%s [%d]\n", room_name, room_id);
            print_to_lobby(out);
            
        }
    }
}

void res_user_list(char *buf, int code)
{
    int i, offset;
    int user_cnt;
    int user_id;
    int user_status;
    char user_name[USER_NAME_MAX_LEN];
    char out[USER_NAME_MAX_LEN + 12];

    if(code == 200)
    {
        print_to_lobby("[*] User list\n");
        buftodata(1, buf, INT, &user_cnt, 8, 4);        
        offset = 12;
        for(i=0 ; i<user_cnt ; i++)
        {
            memset(user_name, 0, USER_NAME_MAX_LEN);
            memset(out, 0, USER_NAME_MAX_LEN + 4);

            buftodata(2, buf, 
                INT, &user_id, offset, 4,
                CHAR, user_name, offset+4, USER_NAME_MAX_LEN,
                INT, &user_status, offset+USER_NAME_MAX_LEN+4, 4);
            offset += (8 + USER_NAME_MAX_LEN);
            cmd_log("\n\n %d", user_status);
            cmd_log("[%d] %s [%d]", user_id, user_name, user_status);
        }
    }
}