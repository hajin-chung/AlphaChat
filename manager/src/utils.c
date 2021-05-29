#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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