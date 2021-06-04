#define CMD_MAX 1000
#define CMD_PRINT_SERVER_INFO "/server"
#define CMD_HELP "/help"
#define CMD_PRINT_ROOMS "/rooms"
#define CMD_PRINT_USERS "/users"
#define CMD_TOGGLE_MCAST "/mcast"

void handle_cmd();
void cmd_help();
void print_room_info();
void print_user_info();
void print_server_info();