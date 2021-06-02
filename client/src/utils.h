typedef enum {
	CHAR,
	INT
} TYPE;

void itoa(int i, char *st);
int atoi_size(char* src, int offset, int size);
void buftodata(int n, char* buf, ...);
void datatobuf(int n, char* buf, ...);