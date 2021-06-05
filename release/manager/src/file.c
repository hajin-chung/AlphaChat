#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "file.h"
#include "utils.h"

void file_init()
{
    // mkdir
    struct stat st = {0};

    if(stat(FILE_PATH, &st) == -1) 
    {
        mkdir(FILE_PATH, 0700);
    } 
}

int new_file_id()
{
    int i;

    for(i=0 ; i<FILE_MAX_CNT ; i++) 
    {
        if(files[i].status == FILE_STATUS_OFF)
        {
            return i;
        }
    }
    return 0;
}

void handle_file_upload_chat(char* buf)
{
    int id;
    int size;
    char name[FILE_NAME_LEN];
    int offset = 12;

    id = new_file_id();
    size = atoi_size(buf, offset, 4);
    offset += 4;
    memcpy(name, buf+offset, FILE_NAME_LEN);

    files[id].id = id;
    files[id].size = size;
    memcpy(files[id].name, name, FILE_NAME_LEN);

    printf("[*] FILE information saved id %d size %d name %s\n", id, size, name);
}

void handle_file_upload_content_req(char* buf)
{
    int id;
    int size;
    char content[FILE_CONTENT_LEN];
    int offset = 0;
    FILE *fp = NULL;

    id = atoi_size(buf, 0, 4);
    offset += 4;
    size = atoi_size(buf, offset, 4);
    offset += 4;
    memcpy(content, buf + offset, size);

    // open file 
    fp = fopen(files[id].name, "wb");

    fwrite(content, 1, size, fp);

    fclose(fp);

    printf("[*] FILE content saved id %d size %d", id, size);
}

void handle_file_download_content_req(char* buf)
{
    
}