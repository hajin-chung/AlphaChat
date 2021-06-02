#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "set.h"
#include "user.h"
#include "room.h"
#include "utils.h"
#include "res.h"
#include "file.h"

void handle_request(int sock, int user_id) 
{
    char buf[MAX_REQ_BUF_SIZE];
    int req_code;
    long len;

    memset(buf, 0, MAX_REQ_BUF_SIZE);

    len = read(sock, buf, MAX_REQ_BUF_SIZE);
    if(len <= 0)
    {
        printf("[*] sock closed of user %d, fd %d\n", sock, user_id);
        close(sock);
        FD_CLR(sock, &fdset);
        users[user_id].status = USER_STATUS_OFFLINE;
        return;
    }
    
    buftodata(1, buf, INT, &req_code, 0, 4);
    printf("[*] New request from %d : code %d\n", user_id, req_code);

    if(req_code == REQ_ROOM_CREATE_CODE)
    {
        room_create(buf, user_id);
    }
    else if(req_code == REQ_ROOM_DELETE_CODE)
    {
        room_delete(buf, user_id);
    }
    else if(req_code == REQ_ROOM_CONNECT_CODE)
    {
        room_connect(buf, user_id);
    }
    else if(req_code == REQ_SEND_CHAT_CODE)
    {
        send_chat(buf, user_id); 
    }
    else if(req_code == REQ_ROOM_INVITE_CODE)
    {
        room_invite(buf, user_id);
    }
    else if(req_code == REQ_REGISTER_CODE)
    {
        user_register(buf, user_id);
    }
    else if(req_code == REQ_ROOM_LIST_CODE)
    {
        room_list(buf, user_id);
    }
    else if(req_code == REQ_USER_LIST_CODE)
    {
        user_list(buf, user_id);
    }
    else if(req_code == REQ_FILE_UPLOAD_CONTENT_CODE)
    {
        handle_file_upload_content_req(buf);
    }
    else if(req_code == REQ_FILE_DOWNLOAD)
    {
        handle_file_download_content_req(buf);
    }
    else
    {
        printf("[!] User %d invalid request code %d\n", user_id, req_code);
    }
}

void user_register(char* buf, int user_id)
{
    char user_name[USER_NAME_MAX_LEN];
    char log[LOG_MAX_LEN];

    memset(user_name, 0, USER_NAME_MAX_LEN);
    
    buftodata(1, buf, CHAR, user_name, 4, USER_NAME_MAX_LEN);

    memcpy(users[user_id].name, user_name, USER_NAME_MAX_LEN);
    
    printf("[*] user_register() user id %d name %s\n", user_id, users[user_id].name);
    
    memset(log, 0, LOG_MAX_LEN);
    sprintf(log, "[*] Success user register id %d, name %s", user_id, user_name);

    response_code(user_id, REQ_REGISTER_CODE, 200, log, LOG_MAX_LEN);
}

void room_create(char* buf, int user_id)
{
    int room_id;
    char room_name[ROOM_NAME_MAX_LEN];
    char log[LOG_MAX_LEN];

    room_id = new_room_id();
    buftodata(1, buf, CHAR, &room_name, 4, ROOM_NAME_MAX_LEN);
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

    response_code(user_id, REQ_ROOM_CREATE_CODE,200, log, LOG_MAX_LEN);
}

void room_delete(char* buf, int user_id)
{
    int room_id;
    char log[104];
    memset(log, 0, LOG_MAX_LEN);

    buftodata(1, buf, INT, &room_id, 4, 4);

    if(user_id == rooms[room_id].super_user_id)
    {
        rooms[room_id].status = ROOM_STATUS_OFF;
        printf("[*] room_delete success room id %d user id %d\n", room_id, user_id);

        sprintf(log, "[*] Success room delete room id %d", room_id);

        response_code(user_id, REQ_ROOM_DELETE_CODE,200, log, LOG_MAX_LEN);
    }
    else 
    {
        printf("[!] room_delete() invalid access : non-super-user tried to delete room\n");

        sprintf(log, "[*] Error room delete invalid access");

        response_code(user_id, REQ_ROOM_DELETE_CODE, 500, log, LOG_MAX_LEN);
    }
}

void room_connect(char* buf, int user_id)
{
    int room_id;
    char log[104];
    memset(log, 0, LOG_MAX_LEN);

    buftodata(1, buf, INT, &room_id, 4, 4);

    if(room_contains_user(room_id, user_id))
    {
        printf("[*] room connect success user id %d to room id %d\n", user_id, room_id);
        res_room_connect(user_id, room_id);
    }
    else
    {
        printf("[!] room_connect() invalid access : non-invited-user tried to connect room\n");

        sprintf(log, "[*] Error room connect invalid access");

        response_code(user_id, REQ_ROOM_CONNECT_CODE, 500, log, LOG_MAX_LEN);
    }
}

void room_invite(char* buf, int user_id)
{
    int offset = 4; 
    int room_id;
    int new_user_id;
    int is_user_in;
    int is_new_user_in;
    char log[LOG_MAX_LEN];
    memset(log, 0, LOG_MAX_LEN);

    buftodata(2, buf,
        INT, &room_id, 4, 4,
        INT, &new_user_id, 8, 4);

    is_user_in = room_contains_user(room_id, user_id);
    is_new_user_in = room_contains_user(room_id, new_user_id);

    printf("[*] user %d invited new user %d to room %d\n", user_id, new_user_id, room_id);

    if(is_user_in && !is_new_user_in)
    {
        rooms[room_id].users[rooms[room_id].user_cnt] = new_user_id;
        rooms[room_id].user_cnt++;
        sprintf(log, "[*] Success room invite new user %d", new_user_id);
        response_code(user_id, REQ_ROOM_INVITE_CODE,200, log, LOG_MAX_LEN);
    }
    else if(!is_user_in)
    {
        sprintf(log, "[!] Fail user %d not contained in room %d", user_id, room_id);
        response_code(user_id, REQ_ROOM_INVITE_CODE, 300, log, LOG_MAX_LEN);
    }
    else if(is_new_user_in)
    {
        sprintf(log, "[!] Fail new user %d already in room %d", new_user_id, room_id);
        response_code(user_id, REQ_ROOM_INVITE_CODE, 300, log, LOG_MAX_LEN);
    }
}

void send_chat(char* buf, int user_id)
{
    int type;
    int room_id;
    char contents[CONTENTS_MAX_LEN];

    int i;
    struct ROOM room;

    memset(contents, 0, CONTENTS_MAX_LEN);

    buftodata(3, buf,
        INT, &type, 4, 4,
        INT, &room_id, 8, 4,
        CHAR, &contents, 12, CONTENTS_MAX_LEN); 

    printf("[*] user %d -> room %d : %s\n", user_id, room_id, contents);
    if(room_contains_user(room_id, user_id))
    {
        room = rooms[room_id];
        for(i=0 ; i<room.user_cnt ; i++)
        {
            response_code(room.users[i], REQ_SEND_CHAT_CODE, 200, buf, MAX_REQ_BUF_SIZE);          
        }
        push_history(room_id, buf);

        if(type == CHAT_TYPE_FILE)
        {
            handle_file_upload_chat(buf);
        }
    }
    else
    {
        printf("[!] Invalid Access user %d tried chat to room %d\n", user_id, room_id);
        response_code(user_id, REQ_SEND_CHAT_CODE, 500, "Error", 5);
    }
}

void user_list(char* buf, int user_id)
{
    res_user_list(user_id);
}

void room_list(char* buf, int user_id)
{
    res_room_list(user_id);
}

