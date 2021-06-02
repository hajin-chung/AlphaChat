// client가 하는일
// 
// 0. splash screen (no name input)
// 1. multicast recv 서버 info(ip, port)
// 2. 서버 접속 
// 3. 이름 입력
// 4. register
// 5. tcp iomux on

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
#include "out.h"
#include "utils.h"
#include "socks.h"
#include "res.h"
#include "req.h"
#include "cmd.h"

struct USER user;

char server_ip[15];
int server_port;
fd_set backup_set, fdset;
int fd_num;
int fd_cnt;
int tcl_sock;
int udp_sock;
int cmd_fd;
struct timeval tv; 

int main()
{
	cmd_fd = fileno(stdin);

    clear_screen();
    print_splash_screen();
    recv_server_info();

    connect_tcp_server();
	get_name();

	FD_ZERO(&fdset);
	FD_SET(tcl_sock, &fdset);
	FD_SET(cmd_fd, &fdset);
	FD_SET(udp_sock, &fdset);

	fd_cnt = tcl_sock;
	while(1)
	{
		backup_set = fdset;
		tv.tv_sec = TIME_VAL_SECONDS;
		tv.tv_usec = 0;

		fd_num = select(fd_cnt + 1, &backup_set, 0, 0, &tv);

		if(fd_num == -1)
		{
			printf("[!] Select ERR");
			exit(0);
		}
		else if(fd_num == 0)
		{
			// send udp heartbeat
		}
		else if(FD_ISSET(tcl_sock, &backup_set))
		{
			handle_res(tcl_sock);
		}
		else if(FD_ISSET(cmd_fd, &backup_set))
		{
			handle_cmd();
		}
	}

}

void recv_server_info()
{
    splash_screen_log("[*] init multicast socket");

    int recv_sock;
	int str_len;
	char buf[MCAST_BUF_SIZE];
	struct sockaddr_in addr;
	struct ip_mreq join_addr;
    int opt = 1;

    recv_sock=socket(PF_INET, SOCK_DGRAM, 0);
 	memset(&addr, 0, sizeof(addr));

	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);	
	addr.sin_port=htons(MULTICAST_PORT);

	setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if(bind(recv_sock, (struct sockaddr*) &addr, sizeof(addr))==-1)	{
		splash_screen_log("[!] multicast socket bind() error");
		close(recv_sock);
		exit(1);	
	}

	join_addr.imr_multiaddr.s_addr=inet_addr(MULTICAST_IP);
	join_addr.imr_interface.s_addr=htonl(INADDR_ANY);
  
	if ( (setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr)))< 0 ) {
		splash_screen_log("[!] multicast setsockopt error");
		close(recv_sock);
		exit(1);
	}
  
    splash_screen_log("[*] Waiting for server info");
    memset(buf, 0, MCAST_BUF_SIZE);
    str_len=recvfrom(recv_sock, buf, MCAST_BUF_SIZE, 0, NULL, 0);
    if(str_len<0) 
    {
        splash_screen_log("[!] multicast socket recvfrom error!");
    }

    buftodata(2, buf,  
        CHAR, server_ip, 0, 15,
        INT, &server_port, 15, 4); 

    splash_screen_log("[*] Server info %s:%d", server_ip, server_port);
}

void connect_tcp_server()
{
	struct sockaddr_in serv_adr;

	tcl_sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(tcl_sock==-1) {
		splash_screen_log("socket() error"); exit(0); }
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(server_ip);
	serv_adr.sin_port=htons(server_port);
	if(connect(tcl_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) 
	{
		splash_screen_log("connect() error!"); exit(0);
	}
	else
	{
		splash_screen_log("[*] TCP Server connected");
	}
}

void get_name()
{
	splash_screen_log(" Name: ");
	fgets(user.name, USER_NAME_MAX_LEN, stdin);
	user.name[strcspn(user.name, "\n")] = 0;
	splash_screen_log(" [*] User name set to %s", user.name);	

	req_register(user.name);
}

void make_udp_socket(int* sock)
{
	
}