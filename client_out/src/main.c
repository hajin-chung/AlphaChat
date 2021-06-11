// CLIENT CORE
//
// 클라이언트가 하는 일들
//  1. 멀티캐스트로 서버의 정보 가져옴
// 	2. 주기적으로 하트비트를 전송
// 	3. 명령어를 받아 서버로 전송
//	4. 서버에서 오는 response를 받아 처리
// 
// 클라이언트 순서
// 	1. 멀티캐스트로 서버의 정보 가져옴
// 	2. tcp로 접속
//	3. 사용자 이름 입력하고 전송
//		4-1. 사용자의 user_id를 전송받음
//	4. 명령어 받아서 처리 및 결과 출력
//		4-1. room connect 명령어 수행시 cur_room_id 바꾸기
//		4-2. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

#include "main.h"
#include "utils.h"
#include "const.h"

#define BUF_SIZE 1024
#define MCAST_BUF_SIZE 30

int DEBUG_FLAG = 0;

char server_ip[30];
int server_port;

int heartbeat_sock, req_sock; // sockets
struct sockaddr_in heartbeat_addr;
int cmd_fd;	// stdin file desc
fd_set fdset, backup_set;

// USER
int my_user_id;
char my_user_name[USER_NAME_MAX_LEN];

// ROOM
int cur_room_id = -1;
int cur_room_name[ROOM_NAME_MAX_LEN];

int heartbeat_log_flag = 0;

int main(int argc, char *argv[])
{
	print_info("[*] Client starting\n");

	char buf[BUF_SIZE];
	struct timeval tv; 
	int fd_cnt, fd_num;

	if(argc!=3) {
		print_info("Usage : %s <MULTICAST IP> <PORT>\n", argv[0]);
		exit(1);
	}
	
	init(argv);

	FD_ZERO(&fdset);
	FD_SET(req_sock, &fdset);
	FD_SET(cmd_fd, &fdset);
	fd_cnt = req_sock;

	req_register();
	
	while(1) 
	{
		backup_set = fdset;

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		fd_num = select(fd_cnt+1, &backup_set, 0, 0, &tv);

		if(fd_num == -1)
		{
			print_error("[!] Unexpected select error");
		}
		else if(fd_num == 0)
		{
			send_heartbeat();
		}
		else if(FD_ISSET(cmd_fd, &backup_set))
		{
			handle_cmd();
		}
		else if(FD_ISSET(req_sock, &backup_set))
		{
			handle_res();
		}
	}
	
	close(req_sock);
	return 0;
}

void init(char* argv[])
{
	// init cmd table
	init_cmd_table();

	// temporary addr
	struct sockaddr_in addr;

	// init cmd_fd
	cmd_fd = fileno(stdin);

	// init multicast sock
	int mcast_sock;
	int str_len;
	char buf[MCAST_BUF_SIZE];
	struct ip_mreq join_addr;
    int opt = 1;

    mcast_sock =socket(PF_INET, SOCK_DGRAM, 0);
 	memset(&addr, 0, sizeof(addr));

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);	
	addr.sin_port=htons(atoi(argv[2])); // set multicast socket port

	setsockopt(mcast_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(mcast_sock, (struct sockaddr*) &addr, sizeof(addr))==-1)
		print_error("[!] multicast socket bind() error");

	memset(&join_addr, 0, sizeof(join_addr));
	join_addr.imr_multiaddr.s_addr=inet_addr(argv[1]);
	join_addr.imr_interface.s_addr=htonl(INADDR_ANY);
  
	if ((setsockopt(mcast_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr)))< 0 ) 
		print_error("[!] multicast setsockopt error");

	// recv server socket info
    print_info("[*] Waiting for server info\n");
    memset(buf, 0, MCAST_BUF_SIZE);
    str_len=recvfrom(mcast_sock, buf, MCAST_BUF_SIZE, 0, NULL, 0);
    if(str_len<0) 
        print_error("[!] multicast socket recvfrom error!");

    buftodata(2, buf,  
        CHAR, server_ip, 0, 15,
        INT, &server_port, 15, 15); 

    print_info("[*] Server info %s:%d\n", server_ip, server_port);
	close(mcast_sock);

	// initialize sockets: heartbeat, req
	// heartbeat
	heartbeat_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(heartbeat_sock==-1)
		print_error("[!] socket() error");
	
	memset(&heartbeat_addr, 0, sizeof(heartbeat_addr));
	heartbeat_addr.sin_family=AF_INET;
	heartbeat_addr.sin_addr.s_addr=inet_addr(server_ip);
	heartbeat_addr.sin_port=htons(UDP_PORT);

	// req
	req_sock=socket(PF_INET, SOCK_STREAM, 0);
    if(req_sock== -1)
        print_error("socket() error");
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(server_ip);
    addr.sin_port=htons(TCP_PORT);

    if(connect(req_sock, (struct sockaddr*)&addr, sizeof(addr))==-1) 
        print_error("connect() error!");
	else
	{
		print_info("[*] REQ socket connected\n");
	}
}

void handle_cmd()
{
	char cmd[CMD_MAX_LEN];
	char buf[MAX_REQ_BUF_SIZE];
	int code, offset;
	int send_flag = 1; // some commands dont send data to server. a flag for that

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	memset(cmd, 0, CMD_MAX_LEN);

	fgets(cmd, CMD_MAX_LEN, stdin);
	remove_newline(cmd, CMD_MAX_LEN);	
	code = cmdtocode(cmd);

	if(code == CMD_HELP)
	{
		send_flag = 0;
		print_help();
	}

	if(code == -1)
	{
		int type = CHAT_TYPE_TEXT;
		code = CMD_SEND_CHAT_CODE;

		if(cur_room_id == -1)
		{
			print_info("[*] room not connected\n");
			send_flag = 0;
		}
		else
		{
			char chat_content[CONTENTS_MAX_LEN];
			memset(chat_content, 0, CONTENTS_MAX_LEN);

			buftodata(1, cmd,
				CHAR, chat_content, 0, CONTENTS_MAX_LEN);

			datatobuf(5, buf, 
				INT, &code, 0, 4,
				INT, &type, 4, 4,
				INT, &cur_room_id, 8, 4,
				INT, &my_user_id, 12, 4,
				CHAR, chat_content, 16, CONTENTS_MAX_LEN);
		}
	}
	else
	{
		offset = strlen(cmd_table[code]);

		if(code == CMD_ROOM_CREATE_CODE) // /create [room_name]
		{
			char room_name[ROOM_NAME_MAX_LEN];
			memset(room_name, 0, ROOM_NAME_MAX_LEN);

			buftodata(1, cmd, 
				CHAR, room_name, offset, ROOM_NAME_MAX_LEN);
			datatobuf(2, buf,
				INT, &code, 0, 4,
				CHAR, room_name, 4, ROOM_NAME_MAX_LEN);
		}
		else if(code == CMD_ROOM_DELETE_CODE) // /delete [room_id]
		{
			int room_id;
			sscanf(cmd+offset, "%d", &room_id);

			datatobuf(2, buf,
				INT, &code, 0, 4,
				INT, &room_id, 4, 4);
		}
		else if(code == CMD_ROOM_CONNECT_CODE) // /connect [room_id]
		{
			int room_id;
			sscanf(cmd+offset, "%d", &room_id);

			datatobuf(2, buf,
				INT, &code, 0, 4,
				INT, &room_id, 4, 4);

			cur_room_id = room_id;
			// 오류 처리는 handle_res쪽에서
		}
		else if(code == CMD_ROOM_INVITE_CODE) // /invite [user_id] [room_id]
		{
			int user_id, room_id;
			sscanf(cmd+offset, "%d %d", &user_id, &room_id);

			datatobuf(3, buf,
				INT, &code, 0, 4,
				INT, &user_id, 4, 4,
				INT, &room_id, 8, 4);
		}
		else if(code == CMD_ROOM_LIST_CODE)
		{
			datatobuf(1, buf,
				INT, &code, 0, 4);
		}
		else if(code == CMD_USER_LIST_CODE)
		{
			datatobuf(1, buf,
				INT, &code, 0, 4);
		}
		else if(code == CMD_TOGGLE_HB_LOG)
		{
			heartbeat_log_flag = !heartbeat_log_flag;
			if(heartbeat_log_flag)
				print_info("[*] Heartbeat log: on\n");
			else
				print_info("[*] Heartbeat log: off\n");

			send_flag = 0;
		}
	}

	if(send_flag)
	{
		if(DEBUG_FLAG)
		{
			print_info("~>");
			write(fileno(stdout), buf, MAX_REQ_BUF_SIZE);
			print_info("\n");
		}
		write(req_sock, buf, MAX_REQ_BUF_SIZE);
	}
}

void handle_res()
{
	char buf[MAX_REQ_BUF_SIZE];
	int str_len;
	int cmd_code=-1, status_code=-1;

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	str_len = read(req_sock, buf, MAX_REQ_BUF_SIZE);
	if(str_len <= 0)
	{
		print_error("[!] Sever closed");
	}

	buftodata(2, buf, 
		INT, &cmd_code, 0, 4,
		INT, &status_code, 4, 4);

	if(DEBUG_FLAG)
	{
		print_info("<~");
		write(fileno(stdout), buf, MAX_REQ_BUF_SIZE);
		print_info("\n");
	}

	if(status_code == CMD_STATUS_FAIL)
	{
		
	}
	else if(cmd_code == CMD_ROOM_LIST_CODE)
	{
		int room_cnt, room_id, offset, i;
		char room_name[ROOM_NAME_MAX_LEN];

		buftodata(1, buf, INT, &room_cnt, 8, 4);

		offset = 12;
		print_info("----------[ROOM LIST %d]----------\n", room_cnt);
		for(i=0 ; i<room_cnt ; i++)
		{
			memset(room_name, 0, ROOM_NAME_MAX_LEN);	
			buftodata(2, buf, 
				INT, &room_id, offset, 4,
				CHAR, room_name, offset+4, ROOM_NAME_MAX_LEN);
			offset += (4 + ROOM_NAME_MAX_LEN);

			print_info("	");
			write(fileno(stdout), room_name, ROOM_NAME_MAX_LEN);
			print_info(" [%d] \n", room_id);
		}
		print_info("----------------------------------\n");
	}
	else if(cmd_code == CMD_USER_LIST_CODE)
	{
		int user_cnt, offset, user_status, user_id, i;
		char user_name[USER_NAME_MAX_LEN];

		buftodata(1, buf, INT, &user_cnt, 8, 4);

		offset = 12;
		print_info("----------[USER LIST %d]----------\n", user_cnt);
		for(i=0 ; i<user_cnt ; i++)
		{
			memset(user_name, 0, USER_NAME_MAX_LEN);	
			buftodata(3, buf, 
				INT, &user_id, offset, 4,
				CHAR, user_name, offset+4, USER_NAME_MAX_LEN,
				INT, &user_status, offset+4+USER_NAME_MAX_LEN, 4);
			offset += (8 + USER_NAME_MAX_LEN);

			print_info("	");
			write(fileno(stdout), user_name, ROOM_NAME_MAX_LEN);
			print_info(" [%d] \n", user_id);
		}
		print_info("----------------------------------\n");
	}
	else if(cmd_code == CMD_ROOM_CONNECT_CODE)
	{

	}
	else if(cmd_code == CMD_SEND_CHAT_CODE)
	{
		int user_id, room_id, chat_type;
		char content[CONTENTS_MAX_LEN];

		memset(content, 0, CONTENTS_MAX_LEN);
		buftodata(4, buf, 
			INT, &chat_type, 12, 4,
			INT, &room_id, 16, 4,
			INT, &user_id, 20, 4,
			CHAR, content, 24, CONTENTS_MAX_LEN);

		if(room_id == cur_room_id)
			print_info("[%d] %s\n", user_id, content);
	}
	else if(cmd_code == CMD_REGISTER_CODE)
	{
		buftodata(1, buf, INT, &my_user_id, 8, 4);	
		print_info("[*] user id : %d\n", my_user_id);
	}
}

void send_heartbeat()
{
	char buf[4];

	memset(buf, 0, 4);
	datatobuf(1, buf, INT, &my_user_id, 0, 4);
	sendto(heartbeat_sock, buf, 4, 0, 
		(struct sockaddr*)&heartbeat_addr, sizeof(heartbeat_addr));

	if(heartbeat_log_flag)
		print_info("[*] heartbeat sent\n");
}

void req_register()
{
	char buf[MAX_REQ_BUF_SIZE];
	char user_name[USER_NAME_MAX_LEN];
	int code = CMD_REGISTER_CODE;

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	memset(user_name, 0, USER_NAME_MAX_LEN);

	print_info("[*] Name: ");
	fgets(user_name, USER_NAME_MAX_LEN, stdin);
	remove_newline(user_name, USER_NAME_MAX_LEN);	

	datatobuf(2, buf, 
		INT, &code, 0, 4,
		CHAR, user_name, 4, USER_NAME_MAX_LEN);

	write(req_sock, buf, MAX_REQ_BUF_SIZE);	
}

void print_help()
{
	printf("---------------[HELP]--------------\n");
    printf("    /help\n        shows help\n");
    printf("    /ul\n        prints user list\n");
    printf("    /rl\n        room list\n");
    printf("    /create [room_name]\n        create room\n");
    printf("    /delete [room_id]\n        delete room\n");
    printf("    /connect [room_id]\n        connect to room\n");
    printf("    /invite [room_id] [user_id]\n        user to room\n");
    printf("-----------------------------------\n");
}