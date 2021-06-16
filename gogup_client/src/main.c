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
#include "print.h"

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
char user_list[1000][100];
char room_list[1000][100];

int main(int argc, char *argv[])
{
	clear_screen();
	print_splash_screen();
	printfSplash("[*] Client starting\n");

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
	
	print_grid();
	move_cursor_cmd();
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
    lobby = initPrintableBox(6, 2, 38, 20);
    chat = initPrintableBox(6, 41, 79, 20);

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
    printfSplash("[*] Waiting for server info\n");
    memset(buf, 0, MCAST_BUF_SIZE);
    str_len=recvfrom(mcast_sock, buf, MCAST_BUF_SIZE, 0, NULL, 0);
    if(str_len<0) 
        print_error("[!] multicast socket recvfrom error!");

    buftodata(2, buf,  
        CHAR, server_ip, 0, 15,
        INT, &server_port, 15, 15); 

    printfSplash("[*] Server info %s:%d\n", server_ip, server_port);
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
		printfSplash("[*] REQ socket connected\n");
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

		if(cmd[0] == '/')
		{
			printfBox(lobby, "[*] %s: Command not found", cmd);
			send_flag = 0;
		}
		else if(cur_room_id == -1)
		{
			printfBox(lobby, "[*] room not connected");
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
			print_room_title("%s %d", cur_room_name, cur_room_id);
			clear_box_buffer(chat);
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
				printfBox(lobby, "[*] Heartbeat log: on");
			else
				printfBox(lobby, "[*] Heartbeat log: off");

			send_flag = 0;
		}
		else if(code == CMD_EXIT)
		{
			exit(0);
		}
	}

	move_cursor_cmd();
	for(int i=0 ; i<115 ; i++)
		printf(" ");
	move_cursor_cmd();

	// clear cmd line

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

	if(status_code == CMD_STATUS_SUCCESS && cmd_code != CMD_SEND_CHAT_CODE)
	{
		printfBox(lobby, "[*] Command %s success", cmd_table[cmd_code]);
	}

	if(status_code == CMD_STATUS_FAIL)
	{
		printfBox(lobby, "[*] Command code %d failed!", cmd_code);		
	}
	else if(cmd_code == CMD_ROOM_LIST_CODE)
	{
		int room_cnt, room_id, offset, i;
		char room_name[ROOM_NAME_MAX_LEN];

		buftodata(1, buf, INT, &room_cnt, 8, 4);

		offset = 12;
		printfBox(lobby, " [ROOM LIST %d]", room_cnt);
		for(i=0 ; i<room_cnt ; i++)
		{
			memset(room_name, 0, ROOM_NAME_MAX_LEN);	
			buftodata(2, buf, 
				INT, &room_id, offset, 4,
				CHAR, room_name, offset+4, ROOM_NAME_MAX_LEN);
			offset += (4 + ROOM_NAME_MAX_LEN);

			memcpy(room_list[room_id], room_name, ROOM_NAME_MAX_LEN);

			printfBox(lobby, "   %s [%d]", room_name, room_id);
		}
	}
	else if(cmd_code == CMD_USER_LIST_CODE)
	{
		int user_cnt, offset, user_status, user_id, i;
		char user_name[USER_NAME_MAX_LEN];

		buftodata(1, buf, INT, &user_cnt, 8, 4);

		offset = 12;
		printfBox(lobby, " [USER LIST %d]", user_cnt);
		for(i=0 ; i<user_cnt ; i++)
		{
			memset(user_name, 0, USER_NAME_MAX_LEN);	
			buftodata(3, buf, 
				INT, &user_id, offset, 4,
				CHAR, user_name, offset+4, USER_NAME_MAX_LEN,
				INT, &user_status, offset+4+USER_NAME_MAX_LEN, 4);
			offset += (8 + USER_NAME_MAX_LEN);

			memcpy(user_list[user_id], user_name, USER_NAME_MAX_LEN);

			printfBox(lobby, "    %s [%d]", user_name, user_id);
		}
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
			printfBox(chat, "%s [%d]> %s", user_list[user_id], user_id, content);
	}
	else if(cmd_code == CMD_REGISTER_CODE)
	{
		buftodata(1, buf, INT, &my_user_id, 8, 4);	
		printfBox(lobby, "[*] user id : %d", my_user_id);
	}
	move_cursor_cmd();
}

void send_heartbeat()
{
	char buf[4];

	memset(buf, 0, 4);
	datatobuf(1, buf, INT, &my_user_id, 0, 4);
	sendto(heartbeat_sock, buf, 4, 0, 
		(struct sockaddr*)&heartbeat_addr, sizeof(heartbeat_addr));

	if(heartbeat_log_flag)
		printfBox(lobby, "[*] heartbeat sent");
}

void req_register()
{
	char buf[MAX_REQ_BUF_SIZE];
	char user_name[USER_NAME_MAX_LEN];
	int code = CMD_REGISTER_CODE;

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	memset(user_name, 0, USER_NAME_MAX_LEN);

	printfSplash("[*] Name: ");
	fgets(user_name, USER_NAME_MAX_LEN, stdin);
	remove_newline(user_name, USER_NAME_MAX_LEN);	

	datatobuf(2, buf, 
		INT, &code, 0, 4,
		CHAR, user_name, 4, USER_NAME_MAX_LEN);

	write(req_sock, buf, MAX_REQ_BUF_SIZE);	
}