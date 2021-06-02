#define SCREEN_WIDTH 120 
#define SCREEN_HEIGHT 28
#define LOBBY_WIDTH 38
#define ROOM_WIDTH 79
#define TITLE_HEIGHT 3
#define OUTPUT_HEIGHT 20
#define CMD_WIDTH 118

#define CMD_LINE SCREEN_HEIGHT - 2
#define NAME_LINE SCREEN_HEIGHT - 3 

void print_grid();
void print_help();
void clear_line(int line_num);

void clear_screen();

void print_splash_screen();
void splash_screen_log(char* log, ...);

void clear_lobby();
void print_to_lobby(char* buf);

void clear_room();
void print_to_room(char* buf);
void print_room_title(char* room_name, int room_id);

void move_cursor(int x, int y);
void move_cursor_cmd();
void move_cursor_name();
