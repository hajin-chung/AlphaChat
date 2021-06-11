#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "const.h"

#define MAX_REQ_BUF_SIZE 1024

char cmd_table[CMD_CNT+1][CMD_MAX_LEN]; 

void init_cmd_table()
{
	strcpy(cmd_table[CMD_ROOM_CREATE_CODE], "/create");
	strcpy(cmd_table[CMD_ROOM_DELETE_CODE], "/delete");
	strcpy(cmd_table[CMD_ROOM_CONNECT_CODE], "/connect");
	strcpy(cmd_table[CMD_ROOM_INVITE_CODE], "/invite");
	strcpy(cmd_table[CMD_SEND_CHAT_CODE], "/send");
	strcpy(cmd_table[CMD_REGISTER_CODE], "/register");
	strcpy(cmd_table[CMD_USER_LIST_CODE], "/ul");
	strcpy(cmd_table[CMD_ROOM_LIST_CODE], "/rl");
	strcpy(cmd_table[CMD_FILE_UPLOAD_CONTENT_CODE], "");
	strcpy(cmd_table[CMD_FILE_DOWNLOAD], "");
	strcpy(cmd_table[CMD_TOGGLE_HB_LOG], "/hb");
	strcpy(cmd_table[CMD_HELP], "/help");
}


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

// converts buf to data
// 	int n : size
//  char* buf
//  (TYPE data_type, void* pointer, int offset, int size)
//                     .
//                     .
//                     .
//  ex) buftodata(2, buf, INT, &id, 0, 4, CHAR, &name, 0, 4, 100)
void buftodata(int n, char* buf, ...)
{
	int i;
	TYPE type;
	void* pointer;
	int offset;
	int size;
	
	va_list ap;

	va_start(ap, buf);
	for(i=0 ; i<n ; i++)
	{
		type = va_arg(ap, TYPE);	
		if(type == CHAR)
		{
			pointer = va_arg(ap, char*);
			offset = va_arg(ap, int);
			size = va_arg(ap, int);
			memcpy(pointer, buf+offset, size);
		}
		else if(type == INT)
		{
			pointer = va_arg(ap, int*);
			offset = va_arg(ap, int);
			size = va_arg(ap, int);
			*((int*)pointer) = atoi(buf+offset);
		}
	}
	va_end(ap);
}

// converts data to buf
// 	int n : size
//  char* buf
//  (TYPE data_type, void* pointer, int offset, int size)
//                     .
//                     .
//                     .
//  ex) buftodata(2, buf, INT, &id, 0, 4, CHAR, &name, 0, 4, 100)
void datatobuf(int n, char* buf, ...)
{
	int i;
	TYPE type;
	void* pointer;
	int offset;
	int size;
	char iii[MAX_REQ_BUF_SIZE];
	
	va_list ap;

	va_start(ap, buf);
	for(i=0 ; i<n ; i++)
	{
		type = va_arg(ap, TYPE);
		if(type == CHAR)
		{
			pointer = va_arg(ap, char*);
			offset = va_arg(ap, int);
			size = va_arg(ap, int);
			memcpy(buf+offset, pointer, size);
		}
		else if(type == INT)
		{
			memset(iii, 0, MAX_REQ_BUF_SIZE);
			pointer = va_arg(ap, int*);
			offset = va_arg(ap, int);
			size = va_arg(ap, int);
			itoa(*((int*)pointer), iii);
			memcpy(buf+offset, iii, size);
		}
	}
	va_end(ap);
}

void print_error(char* buf, ...)
{
	va_list ap;
	va_start(ap, buf);
	vprintf(buf, ap);
	fflush(stdout);
	va_end(ap);
	exit(0);
}

void print_info(char* buf, ...)
{
	va_list ap;
	va_start(ap, buf);
	vprintf(buf, ap);
	fflush(stdout);
	va_end(ap);
}

void print_warning(char* buf, ...)
{
	va_list ap;
	va_start(ap, buf);
	vprintf(buf, ap);
	fflush(stdout);
	va_end(ap);
}

void print_success(char* buf, ...)
{
	va_list ap;
	va_start(ap, buf);
	vprintf(buf, ap);
	fflush(stdout);
	va_end(ap);
}

int cmdtocode(char* cmd)
{
	int i;
	int len;

	for(i=1 ; i<=CMD_CNT ; i++)
	{
		len = strlen(cmd_table[i]);
		if(len >0 && strncmp(cmd_table[i], cmd, len) == 0)
			return i;
	}
	return -1;
}

void remove_newline(char* buf, int size)
{
	int idx = strcspn(buf, "\n");

	if(idx < size)
		buf[idx] = '\0';
}