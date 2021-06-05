// CLIENT CORE
//
// 클라이언트가 하는 일들
//  1. 멀티캐스트로 서버의 정보 가져옴
// 	2. 주기적으로 하트비트를 전송
// 	3. 명령어를 받아 서버로 전송
//	4. 서버에서 오는 response를 받아 처리

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

#define BUF_SIZE 1024
#define MCAST_BUF_SIZE 30

char server_ip[30];
int server_port;

int heartbeat_sock, req_sock; // sockets
struct sockaddr_in heartbeat_addr;
int cmd_fd;	// stdin file desc
fd_set fdset, backup_set;

// USER
int user_id;
char user_name[USER_NAME_MAX_LEN];

int main(int argc, char *argv[])
{
	printf("[*] Client starting\n");
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
	char buf[MAX_REQ_BUF_SIZE];

	memset(buf, 0, BUF_SIZE);
	fgets(buf, BUF_SIZE, stdin);
	buf[strcspn(buf, "\n")] = 0;
	if(buf[0]!=0)
		print_info("<~ ");
	write(fileno(stdout), buf, BUF_SIZE);
	print_info("\n");

	write(req_sock, buf, strlen(buf));
}

void handle_res()
{
	char buf[MAX_REQ_BUF_SIZE];
	int str_len;

	memset(buf, 0, BUF_SIZE);
	str_len=read(req_sock, buf, BUF_SIZE);
	if(str_len <= 0)
	{
		print_error("[!] Sever closed");
	}
	print_info("~> ");
	write(fileno(stdout), buf, BUF_SIZE);
	print_info("\n");
}

void send_heartbeat()
{
	char buf[4];

	memset(buf, 0, 4);
	datatobuf(1, buf, INT, &user_id, 0, 4);
	sendto(heartbeat_sock, buf, 4, 0, 
		(struct sockaddr*)&heartbeat_addr, sizeof(heartbeat_addr));

	print_info("[*] heartbeat sent\n");
}