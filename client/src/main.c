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

struct USER user;

char server_ip[15];
int server_port;
fd_set backup_set, fdset;
int fd_num;
int fd_cnt;
int tcl_sock;
int udp_sock;
int stdin_fd = fileno(stdin);
struct timeval tv; 

int main()
{
    clear_screen();
    print_splash_screen();
    recv_server_info();

    connect_tcp_server();
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
	tcl_sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(tcl_sock==-1) {
		splash_screen_log("socket() error"); exit(0); }
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) {
		splash_screen_log("connect() error!"); exit(0);
	}
	else
		splash_screen_log("[*] TCP Server connected");
}