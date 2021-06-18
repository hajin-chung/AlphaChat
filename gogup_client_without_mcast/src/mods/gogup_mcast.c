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

#include "../main.h"
#include "../const.h"
#include "../cmd.h"
#include "../res.h"
#include "../print.h"
#include "../utils.h"

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