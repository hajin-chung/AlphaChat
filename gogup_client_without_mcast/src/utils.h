typedef enum {
	CHAR,
	INT
} TYPE;

void itoa(int i, char *st);
int atoi_size(char* src, int offset, int size);
void buftodata(int n, char* buf, ...);
void datatobuf(int n, char* buf, ...);
void print_error(char* buf, ...);
void print_info(char* buf, ...);
void print_warning(char* buf, ...);
void print_success(char* buf, ...);
int cmdtocode(char* cmd);
void init_cmd_table();
void remove_newline(char* buf, int size);