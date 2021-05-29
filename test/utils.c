#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void itoa(int i, char *st)
{
	sprintf(st, "%d", i);
	return;
}

int atoi_size(char* src, int offset, int size)
{
	int len = strlen(src);
	char* dest;

	dest = malloc(len);
	memcpy(dest, src+offset, size);
	return atoi(dest);
}