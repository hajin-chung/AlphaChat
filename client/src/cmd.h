#define CMD_MAX_LEN 300

#define CMD_HELP "/help"
#define CMD_ROOM_LIST "/rl"
#define CMD_USER_LIST "/ul"
#define CMD_ROOM_CREATE "/create"


void init_lobby();
int check_bufecmd(char* buf, char* cmd);
void handle_cmd();

void room_list();