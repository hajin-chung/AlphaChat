#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "cmd.h"
#include "user.h"
#include "room.h"
#include "main.h"

void handle_cmd()
{
    char buf[CMD_MAX];
    memset(buf, 0, CMD_MAX);

    scanf("%s", buf);

    if(strcmp(buf, CMD_HELP) == 0)
    {
        cmd_help();
    }
    else if(strcmp(buf, CMD_PRINT_SERVER_INFO) == 0)
    {
        print_server_info();
    }
    else if(strcmp(buf, CMD_PRINT_ROOMS) == 0)
    {
        print_room_info();
    }
    else if(strcmp(buf, CMD_PRINT_USERS) == 0)
    {
        print_user_info();
    }
    else if(strcmp(buf, CMD_TOGGLE_MCAST) == 0)
    {
        multicast_log_flag = !multicast_log_flag;
        if(multicast_log_flag)
        {
            printf("[*] multicast log: on\n");
        }
        else
        {
            printf("[*] multicast log: off\n");
        }
    }
}

void cmd_help()
{
    printf("---------------[HELP]-------------\n");
    printf("    /help   shows help\n");
    printf("    /server prints server info\n");
    printf("    /rooms  prints rooms info\n");
    printf("    /users  prints users info\n");
    printf("    /mcast  toggle on/off mast log\n");
    printf("----------------------------------\n");
}

void print_room_info()
{
	int i, j;
	printf("\n-------------[ROOM INFO]-------------\n");
	printf("    room cnt: %d\n", rooms_cnt);
	for(i=0 ; i<rooms_cnt ; i++)
	{
        printf("\n");
		printf("\n    ---------[ROOM %d]---------\n", i);
		printf("        name: %s\n", rooms[i].name);
		printf("        id: %d\n", rooms[i].id);
		printf("        su: %d\n", rooms[i].super_user_id);
		printf("        status: %d\n", rooms[i].status);
		printf("        user cnt: %d\n", rooms[i].user_cnt);
		printf("        users: ");
		for(j=0 ; j<rooms[i].user_cnt ; j++)
		{
			printf("%d, ", rooms[i].users[j]);
		}
		printf("\n");
        printf("        history cnt: %d\n", rooms[i].history_cnt);
        printf("        history: \n");
        for(j=0 ; j<rooms[i].history_cnt ; j++)
        {
            printf("        ------[CHAT]------\n");
            printf("            ");
            fwrite(rooms[i].history[j], 1, MAX_REQ_BUF_SIZE, stdout);
            printf("\n");
            printf("        ------------------\n");
        }
		printf("    ---------------------------\n");
	}	
	printf("-------------------------------------\n");
}

void print_user_info()
{
	int i, j;
	printf("\n-------------[USER INFO]-------------\n");
	printf("    user cnt: %d\n", users_cnt);
	for(i=0 ; i<users_cnt ; i++)
	{
        printf("\n");
		printf("    ---------[user %d]---------\n", i);
		printf("        name: %s\n", users[i].name);
		printf("        id: %d\n", users[i].id);
		printf("        status: %d\n", users[i].status);
		printf("        sock: %d\n", users[i].sock);
		printf("    ---------------------------\n");
	}	
	printf("-------------------------------------\n");
}

void print_server_info()
{
    printf("\n----------------[INFO]----------------\n");
    printf("    MULTICAST : %s:%d\n", MULTICAST_IP, MULTICAST_PORT);
    printf("    SERVER    : %s:%d\n", SERVER_IP, TCP_PORT);
    printf("    HEARDBEAT : %d\n", UDP_PORT);
    printf("--------------------------------------\n");
}