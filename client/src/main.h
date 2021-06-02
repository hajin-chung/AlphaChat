
#include "const.h"

#define MCAST_BUF_SIZE 30

struct USER {
    int id;
    char name[USER_NAME_MAX_LEN];
    int status;
};

extern struct USER user;

void recv_server_info();
void connect_tcp_server();