#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

int fd_cnt;
fd_set fdset, backup_set;