#define FILE_PATH "./files"
#define FILE_MAX_CNT 1000
#define FILE_CONTENT_LEN 1000
#define FILE_NAME_LEN 100
#define FILE_STATUS_ON 1
#define FILE_STATUS_OFF 0

struct File {
    int status;
    int id;
    char name[FILE_NAME_LEN];
    int size;
};
int file_cnt;

struct File files[FILE_MAX_CNT];

void file_init();
int new_file_id();
void handle_file_upload_chat(char* buf);
void handle_file_upload_content_req(char* buf);
void handle_file_download_content_req(char* buf);