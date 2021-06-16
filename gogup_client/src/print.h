struct printableBox {
    int sy, sx;
    int w, h;
    char* buffer;
    int size;
    int py, px;
    int sp;
    int linepos;
};

extern struct printableBox* lobby;
extern struct printableBox* chat;

void clear_screen();
void move_cursor(int y, int x);
void move_cursor_cmd();
struct printableBox* initPrintableBox(int sy, int sx, int w, int h);
void printToBox(struct printableBox* box, char* s, int n);
void init_print(struct printableBox* lobby, struct printableBox* chat);
void print_grid();
void print_help();
void printfBox(struct printableBox* box, char* s, ...);
void update_screen(struct printableBox* lobby, struct printableBox* chat);
void print_room_title(char* s, ...);
void clear_box_buffer(struct printableBox* box);
void print_splash_screen();
void printfSplash(char* s, ...);