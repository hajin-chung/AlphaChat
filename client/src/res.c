#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "res.h"

void handle_res(int sock)
{
    char buf[MAX_REQ_BUF_SIZE];
    int res_code;
    long len;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    len = read(sock, buf, MAX_REQ_BUF_SIZE);
    if(len <= 0)
    {
        return;
    }

    buftodata(1, buf, INT, &req_code, )
}