#include "const.h"

struct USER {
    int id;
    char name[USER_NAME_MAX_LEN];
    int status;
    int sock;
};

struct USER users[USER_MAX];
int users_cnt;

int new_user_id();