#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "room.h"
#include "user.h"

void itoa(int i, char *st)
{
	sprintf(st, "%d", i);
	return;
}

int atoi_size(char* src, int offset, int size)
{
	int len = strlen(src);
	char* dest;
	int ans;

	dest = malloc(len);
	memcpy(dest, src+offset, size);
	ans = atoi(dest);

	free(dest);
	return ans;
}

void print_room_info()
{
	int i, j;
	printf("\n-------------[ROOM INFO]-------------\n");
	printf("	room cnt: %d\n", rooms_cnt);
	for(i=0 ; i<rooms_cnt ; i++)
	{
		printf("\n	---------[ROOM %d]---------\n", i);
		printf("		name: %s\n", rooms[i].name);
		printf("		su: %d\n", rooms[i].super_user_id);
		printf("		status: %d\n", rooms[i].status);
		printf("		user cnt: %d\n", rooms[i].user_cnt);
		printf("		users: ");
		for(j=0 ; j<rooms[i].user_cnt ; j++)
		{
			printf("%d, ", rooms[i].users[j]);
		}
		printf("\n");
		printf("	---------------------------\n");
	}	
	printf("-------------------------------------\n");
}

void print_user_info()
{
	int i, j;
	printf("\n-------------[USER INFO]-------------\n");
	printf("	user cnt: %d\n", users_cnt);
	for(i=0 ; i<users_cnt ; i++)
	{
		printf("\n	---------[user %d]---------\n", i);
		printf("		name: %s\n", users[i].name);
		printf("		id: %d\n", users[i].id);
		printf("		status: %d\n", users[i].status);
		printf("		sock: %d\n", users[i].sock);
		printf("	---------------------------\n");
	}	
	printf("-------------------------------------\n");
}

void print_server_info()
{
    printf("[*] Server starting\n");
    printf("\n----------------[INFO]----------------\n");
    printf("    MULTICAST : %s:%d\n", MULTICAST_IP, MULTICAST_PORT);
    printf("    SERVER    : %s:%d\n", SERVER_IP, TCP_PORT);
    printf("    HEARDBEAT : %d\n", UDP_PORT);
    printf("--------------------------------------\n");
}