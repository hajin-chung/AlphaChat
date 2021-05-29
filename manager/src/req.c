#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "set.h"
#include "user.h"
#include "room.h"
#include "utils.h"
#include "res.h"

void handle_request(int sock, int uid) 
{
    char buf[MAX_REQ_BUF_SIZE];
    char req[MAX_REQ_BUF_SIZE];
    char req_code_raw[4];
    int req_code;
    long len;

    memset(buf, 0, MAX_REQ_BUF_SIZE);
    memset(req, 0, MAX_REQ_BUF_SIZE);
    memset(req_code_raw, 0, 4);

    len = read(sock, buf, MAX_REQ_BUF_SIZE);
    if(len <= 0)
    {
        printf("[*] sock closed of user %d, fd %d\n", sock, uid);
        close(sock);
        FD_CLR(sock, &fdset);
        users[uid].status = USER_STATUS_OFFLINE;
        return;
    }
    
    memcpy(req_code_raw, buf, 4); 
    req_code = atoi(req_code_raw);
    printf("[*] New request from %d : code %d\n", uid, req_code);

    if(req_code == REQ_ROOM_CREATE_CODE)
    {
        room_create(buf, uid);
    }
    else if(req_code == REQ_ROOM_DELETE_CODE)
    {
        room_delete(buf, uid);
    }
    else if(req_code == REQ_ROOM_CONNECT_CODE)
    {
        room_connect(buf, uid);
    }
    else if(req_code == REQ_SEND_CHAT_CODE)
    {
        send_chat(buf, uid); 
    }
    else if(req_code == REQ_ROOM_INVITE_CODE)
    {
        room_invite(buf, uid);
    }
    else if(req_code == REQ_REGISTER_CODE)
    {
        user_register(buf, uid);
    }
    else if(req_code == REQ_ROOM_LIST_CODE)
    {
        room_list(buf, uid);
    }
    else if(req_code == REQ_USER_LIST_CODE)
    {
        user_list(buf, uid);
    }
    else
    {
        printf("[!] User %d invalid request code %d\n", uid, req_code);
    }
}

void user_register(char* buf, int uid)
{
    int offset = 4;
    char user_name[USER_NAME_MAX_LEN];
    char log[LOG_MAX_LEN];

    memset(user_name, 0, USER_NAME_MAX_LEN);
    
    memcpy(user_name, buf+offset, USER_NAME_MAX_LEN);

    memcpy(users[uid].name, user_name, USER_NAME_MAX_LEN);
    
    printf("[*] user_register() user id %d name %s\n", uid, users[uid].name);
    
    memset(log, 0, LOG_MAX_LEN);
    sprintf(log, "[*] Success user register id %d, name %s", uid, user_name);

    response_code(uid, 200, log, LOG_MAX_LEN);
}

void room_create(char* buf, int uid)
{
    int offset = 4;
    int room_id;
    int user_id;
    char room_name[ROOM_NAME_MAX_LEN];
    char log[LOG_MAX_LEN];

    room_id = new_room_id();
    user_id = atoi_size(buf, offset, 4);
    offset += 4;
    memcpy(room_name, buf+offset, ROOM_NAME_MAX_LEN);

    memset(&rooms[room_id], 0, sizeof(struct ROOM));

    rooms[room_id].status = ROOM_STATUS_ON;
    rooms[room_id].id = room_id;
    rooms[room_id].super_user_id = user_id;
    memcpy(rooms[room_id].name, room_name, ROOM_NAME_MAX_LEN);
    rooms[room_id].user_cnt++;
    rooms[room_id].users[0] = user_id; 
    if(room_id+1 > rooms_cnt)
    {
        rooms_cnt = room_id+1;
    }

    printf("[*] room_create() room id %d room name %s user id %d\n", room_id, room_name, user_id);

    memset(log, 0, LOG_MAX_LEN);
    sprintf(log, "[*] Success room create room id %d, room name %s", room_id, room_name);

    response_code(uid, 200, log, LOG_MAX_LEN);

}

void room_delete(char* buf, int uid)
{
    int offset = 4;
    int room_id;
    int user_id;
    char log[104];
    memset(log, 0, LOG_MAX_LEN);

    user_id = atoi_size(buf, offset, 4);
    offset += 4;
    room_id = atoi_size(buf, offset, 4);
    offset += 4;

    if(user_id == rooms[room_id].super_user_id)
    {
        rooms[room_id].status = ROOM_STATUS_OFF;
        printf("[*] room_delete success room id %d user id %d\n", room_id, user_id);

        sprintf(log, "[*] Success room delete room id %d", room_id);

        response_code(uid, 200, log, LOG_MAX_LEN);
    }
    else 
    {
        printf("[!] room_delete() invalid access : non-super-user tried to delete room\n");

        sprintf(log, "[*] Error room delete invalid access");

        response_code(uid, 500, log, LOG_MAX_LEN);
    }
}

void room_connect(char* buf, int uid)
{
    int offset = 4;
    int user_id;
    int room_id;

    user_id = atoi_size(buf, offset, 4);
    offset += 4;
    room_id = atoi_size(buf, offset, 4);
    offset += 4;

    if(room_contains_user(room_id, user_id))
    {
        // send user chat history
    }
    else
    {
        // send user invalid access
    }
}

void room_invite(char* buf, int uid)
{
    int offset = 4; 
    int user_id;
    int room_id;
    int new_user_id;
    int is_user_in;
    int is_new_user_in;
    char log[LOG_MAX_LEN];
    memset(log, 0, LOG_MAX_LEN);

    user_id = atoi_size(buf, offset, 4);
    offset += 4;
    room_id = atoi_size(buf, offset, 4);
    offset += 4;
    new_user_id = atoi_size(buf, offset, 4);
    offset += 4;

    is_user_in = room_contains_user(room_id, user_id);
    is_new_user_in = room_contains_user(room_id, new_user_id);

    printf("[*] user %d invited new user %d to room %d\n", user_id, new_user_id, room_id);

    if(is_user_in && !is_new_user_in)
    {
        rooms[room_id].users[rooms[room_id].user_cnt] = new_user_id;
        rooms[room_id].user_cnt++;
        sprintf(log, "[*] Success room invite new user %d", new_user_id);
        response_code(uid, 200, log, LOG_MAX_LEN);
    }
    else if(!is_user_in)
    {
        sprintf(log, "[!] Fail user %d not contained in room %d", user_id, room_id);
        response_code(uid, 300, log, LOG_MAX_LEN);
    }
    else if(is_new_user_in)
    {
        sprintf(log, "[!] Fail new user %d already in room %d", new_user_id, room_id);
        response_code(uid, 300, log, LOG_MAX_LEN);
    }
}

void send_chat(char* buf, int uid)
{
    int type;
    int user_id;
    int room_id;
    char contents[CONTENTS_MAX_LEN];
    struct SendChat chat;
    int offset = 4;

    int i;
    struct ROOM room;

    memset(contents, 0, CONTENTS_MAX_LEN);
    memset(&chat, 0, sizeof(struct SendChat));

    type = atoi_size(buf, offset, 4);
    offset+=4;
    user_id = atoi_size(buf, offset, 4);
    offset+=4;
    room_id = atoi_size(buf, offset, 4);
    offset+=4;
    memcpy(contents, buf+offset, CONTENTS_MAX_LEN);
    offset+=CONTENTS_MAX_LEN;

    chat.type = type;
    chat.user_id = user_id;
    chat.room_id = room_id;
    memcpy(chat.contents, contents, CONTENTS_MAX_LEN);

    printf("[*] user %d -> room %d : %s\n", user_id, room_id, contents);
    if(room_contains_user(room_id, user_id))
    {
        room = rooms[room_id];
        for(i=0 ; i<room.user_cnt ; i++)
        {
            send_to_user(room.users[i], buf+4, 4*3+CONTENTS_MAX_LEN);          
        }
        // push to history
    }
    else
    {
        printf("[!] Invalid Access user %d tried chat to room %d", user_id, room_id);
        response_code(uid, 500, "Error", 5);
    }
}

void user_list(char* buf, int uid)
{
    res_user_list(uid);
}

void room_list(char* buf, int uid)
{
    res_room_list(uid);
}