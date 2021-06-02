#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socks.h"
#include "const.h"
#include "utils.h"

void send_to_server(char* buf, int size)
{
    write(tcl_sock, buf, size);
}

void req_register(char* user_name)
{
    char buf[MAX_REQ_BUF_SIZE];
    int code = 5;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    datatobuf(2, buf, 
        INT, &code, 0, 4,
        CHAR, user_name, 4, USER_NAME_MAX_LEN);

    send_to_server(buf, MAX_REQ_BUF_SIZE);
}