#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
}