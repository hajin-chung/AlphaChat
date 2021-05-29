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

#define BUF_SIZE 5120

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
    fd_set backup_set, fdset;
	int fd_num;
	int fd_cnt;
	int stdin_fd = fileno(stdin);
	struct timeval tv; 

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1) {
		printf("socket() error"); exit(0); }
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) {
		printf("connect() error!"); exit(0); }
	else
		printf("Connected...........\n");

	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	FD_SET(stdin_fd, &fdset);
	fd_cnt = sock;
	
	while(1) 
	{
		backup_set = fdset;

		tv.tv_sec = 5;
		tv.tv_usec = 0;

		fd_num = select(fd_cnt+1, &backup_set, 0, 0, &tv);

		if(fd_num == -1)
		{
			printf("error!\n");
		}
		else if(FD_ISSET(stdin_fd, &backup_set))
		{
			memset(message, 0, BUF_SIZE);
			fgets(message, BUF_SIZE, stdin);
			message[strcspn(message, "\n")] = 0;
			printf("<~ ");
			fwrite(message, 1, BUF_SIZE, stdout);
			printf("\n");
			
			if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
				break;

			write(sock, message, strlen(message));
		}
		else if(FD_ISSET(sock, &backup_set))
		{
			str_len=read(sock, message, BUF_SIZE);
			if(str_len <= 0)
			{
				printf("!! SERVER CLOSED !!\n\n");
				return 0;
			}
			printf("~> ");
			fwrite(message, 1, BUF_SIZE, stdout);
			printf("\n");
		}

	}
	
	close(sock);
	return 0;
}
