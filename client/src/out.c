#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "out.h"
#include "main.h"

char room_buf[OUTPUT_HEIGHT][ROOM_WIDTH];
char lobby_buf[OUTPUT_HEIGHT][LOBBY_WIDTH];
int lobby_buf_y = 0, lobby_buf_x = 0;

void print_help()
{
    clear_screen();
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    printf("|   __    __            __                                                                                             |\n");
    printf("|  /  |  /  |          /  |                                                                                            |\n");
    printf("|  $$ |  $$ |  ______  $$ |  ______                                                                                    |\n");
    printf("|  $$ |__$$ | /      \\ $$ | /      \\                                                                                   |\n");
    printf("|  $$    $$ |/$$$$$$  |$$ |/$$$$$$  |                                                                                  |\n");
    printf("|  $$$$$$$$ |$$    $$ |$$ |$$ |  $$ |                                                                                  |\n");
    printf("|  $$ |  $$ |$$$$$$$$/ $$ |$$ |__$$ |                                                                                  |\n");
    printf("|  $$ |  $$ |$$       |$$ |$$    $$/                                                                                   |\n");
    printf("|  $$/   $$/  $$$$$$$/ $$/ $$$$$$$/                                                                                    |\n");
    printf("|                          $$ |                                                                                        |\n");
    printf("|                          $$ |                                                                                        |\n");
    printf("|                          $$/                                                                                         |\n");
    printf("|  ---Lobby Command---                                                                                                 |\n");
    printf("|  /help = Show this page.                                                                                             |\n");
    printf("|  /create [RoomName] = Create Room [RoomName] and allocate RoomID.                                                    |\n");
    printf("|  /delete [RoomID] = Delete Room [RoomID].                                                                            |\n");
    printf("|  /coonect [RoomID] = Get [RoomID]'s Chat History and set your target your chat to [RoomID].                          |\n");
    printf("|  /invite [ID] [RoomID] = Invite [ID] to [RoomID].                                                                    |\n");
    printf("|  /cs = Show Client Status.                                                                                           |\n");
    printf("|  /rl = Show Room List                                                                                                |\n");
    printf("|  ---Room Command---                                                                                                  |\n");
    printf("|  !exit = Exit the room.                                                                                              |\n");
    printf("|  !emoji [EmojiName] = Send Emoji.                                                                                    |\n");
    printf("|  !file [Filepath] = Send File profile to Server.                                                                     |\n");
    printf("|                                                                                                                      |\n");
    printf("|  Press ENTER to exit...                                                                                              |\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n"); 
    fflush(stdout);
    while((getchar() == EOF));
    char id;
    scanf(" %c",&id);
}

void clear_lobby()
{
    memset(lobby_buf, 0, OUTPUT_HEIGHT * LOBBY_WIDTH);

}

void print_to_lobby(char* buf)
{
    
}

void print_lobby_buf()
{
    move_cursor()
}

void room_printf(char* buf, ...)
{
    clear_room();
    print_to_room(buf);
}

void clear_room()
{
    char blank[ROOM_WIDTH * OUTPUT_HEIGHT];
    memset(blank, ' ', ROOM_WIDTH * OUTPUT_HEIGHT);

    print_to_room(blank);
}

void print_to_room(char* buf)
{
    int buf_pos = 0;
    int line_cnt = 0;
    int new_line_pos;
    char line[ROOM_WIDTH];
    int line_len;

    while(line_cnt < OUTPUT_HEIGHT)
    {
        memset(line, 0, ROOM_WIDTH);
        memcpy(line, buf+buf_pos, ROOM_WIDTH-2);
        line_len = strlen(line);
        new_line_pos = strcspn(line, "\n");

        move_cursor(5 + line_cnt, 3 + LOBBY_WIDTH);
        if(line_len == new_line_pos)
        {
            printf("%s", line);
            buf_pos += line_len;
        }
        else
        {
            line[new_line_pos] = '\0';
            printf("%s", line);
            buf_pos += strlen(line) + 1;
        }
        fflush(stdout);

        line_cnt++;
    }
}

void splash_screen_log(char* log, ...)
{
    int i;
    move_cursor_name();
    for(i=0 ; i<SCREEN_WIDTH - 5 ; i++) printf(" ");
    fflush(stdout);
    move_cursor_name();
    va_list args;
    va_start(args, log);
    vprintf(log, args);
    va_end(args);
    fflush(stdout);
}

void cmd_log(char* log, ...)
{
    move_cursor(SCREEN_HEIGHT + 1, 2);
    va_list args;
    va_start(args, log);
    vprintf(log, args);
    va_end(args);
    fflush(stdout);
    move_cursor_cmd();
}

void clear_screen()
{
    printf("\033[2J");
    printf("\033[H");
    fflush(stdout);
}

void move_cursor(int y, int x)
{
    printf("\033[%d;%dH", y+1,x+1);
    fflush(stdout);
}

void move_cursor_name()
{
    move_cursor(NAME_LINE, 2);
}

void move_cursor_cmd()
{
    int i;
    move_cursor(CMD_LINE, 4);
    for(i=0 ; i<CMD_WIDTH-3 ; i++) printf(" ");
    move_cursor(CMD_LINE, 4);
    fflush(stdout);
}

void print_room_title(char* room_name, int room_id)
{
    int len;
    int title_start;
    char* title;
    
    title = malloc(LOBBY_WIDTH);
    memset(title, 0, LOBBY_WIDTH);
    sprintf(title, "%s (%d)", room_name, room_id);
    len = strlen(title);

    title_start = 2 + LOBBY_WIDTH + ROOM_WIDTH / 2 - len / 2;
    
    move_cursor(2, title_start);
    printf("%s", title);
    fflush(stdout);
}

void print_splash_screen()
{
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                ______   __            __                         ______   __                    __                   |\n");
    printf("|               /      \\ /  |          /  |                       /      \\ /  |                  /  |                  |\n");
    printf("|              /$$$$$$  |$$ |  ______  $$ |____    ______        /$$$$$$  |$$ |____    ______   _$$ |_                 |\n");
    printf("|              $$ |__$$ |$$ | /      \\ $$      \\  /      \\       $$ |  $$/ $$      \\  /      \\ / $$   |                |\n");
    printf("|              $$    $$ |$$ |/$$$$$$  |$$$$$$$  | $$$$$$  |      $$ |      $$$$$$$  | $$$$$$  |$$$$$$/                 |\n");
    printf("|              $$$$$$$$ |$$ |$$ |  $$ |$$ |  $$ | /    $$ |      $$ |   __ $$ |  $$ | /    $$ |  $$ | __               |\n");
    printf("|              $$ |  $$ |$$ |$$ |__$$ |$$ |  $$ |/$$$$$$$ |      $$ \\__/  |$$ |  $$ |/$$$$$$$ |  $$ |/  |              |\n");
    printf("|              $$ |  $$ |$$ |$$    $$/ $$ |  $$ |$$    $$ |      $$    $$/ $$ |  $$ |$$    $$ |  $$  $$/               |\n");
    printf("|              $$/   $$/ $$/ $$$$$$$/  $$/   $$/  $$$$$$$/        $$$$$$/  $$/   $$/  $$$$$$$/    $$$$/                |\n");
    printf("|                            $$ |                                                                                      |\n");
    printf("|                            $$ |                                                                                      |\n");
    printf("|                            $$/                                                                                       |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("|                                                                                                                      |\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    fflush(stdout);
}

void print_grid()
{
    clear_screen();
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    printf("|                                      |                                                                               |\n");
    printf("|                LOBBY                 |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|--------------------------------------|-------------------------------------------------------------------------------|\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("|----------------------------------------------------------------------------------------------------------------------|\n");
    printf("| >                                                                                                                    |\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    fflush(stdout);
}