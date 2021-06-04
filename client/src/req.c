#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socks.h"
#include "const.h"
#include "const.h"
#include "utils.h"
#include "out.h"

void send_to_server(char* buf, int size)
{
    write(tcl_sock, buf, size);
}

void req_register(char* user_name)
{
    char buf[MAX_REQ_BUF_SIZE];
    int code = REQ_REGISTER_CODE;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    datatobuf(2, buf, 
        INT, &code, 0, 4,
        CHAR, user_name, 4, USER_NAME_MAX_LEN);

    send_to_server(buf, MAX_REQ_BUF_SIZE);
}

void req_room_list()
{
    char buf[MAX_REQ_BUF_SIZE];
    int code = REQ_ROOM_LIST_CODE;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    datatobuf(1, buf,
        INT, &code, 0, 4);

    send_to_server(buf, MAX_REQ_BUF_SIZE);
}

void req_user_list()
{
    char buf[MAX_REQ_BUF_SIZE];
    int code = REQ_USER_LIST_CODE;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    datatobuf(1, buf,
        INT, &code, 0, 4);

    send_to_server(buf, MAX_REQ_BUF_SIZE);
}

void req_room_create(char* cmd)
{
    char buf[MAX_REQ_BUF_SIZE];
    char new_room_name[ROOM_NAME_MAX_LEN];
    int code = REQ_ROOM_CREATE_CODE;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    sscanf(cmd, "/create %s", new_room_name);
    cmd_log("[*] cmd room create %s", new_room_name);

    datatobuf(2, buf,
        INT, &code, 0, 4,
        CHAR, new_room_name, 4, ROOM_NAME_MAX_LEN);

    send_to_server(buf, MAX_REQ_BUF_SIZE);
}