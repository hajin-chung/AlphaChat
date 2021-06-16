#include "print.h"
#include "const.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

struct printableBox* lobby;
struct printableBox* chat;

void move_cursor(int y, int x)
{
    printf("\033[%d;%dH", y, x);
    fflush(stdout);
}

void move_cursor_cmd()
{
	move_cursor(27, 5);
}

struct printableBox* initPrintableBox(int sy, int sx, int w, int h)
{
    struct printableBox* box = malloc(sizeof(struct printableBox));
    memset(box, 0, sizeof(struct printableBox));

    box->sy = sy;
    box->sx = sx;
    box->w = w;
    box->h = h;
    box->buffer = malloc(w*h);
    box->size = w*h;
    box->sp = 0;

    return box;
}

void printToBox(struct printableBox* box, char* s, int n)
{
    int i, j, tmp;
    int pos;

    pos = box->sp;
    for(i=0 ; i<n ; i++)
    {
        // if s[i] = newline 
        if(s[i] == 10) // newline
        {
            tmp = (int)((int)(pos+box->w)/box->w)*box->w - pos;
            for(j=0 ; j<tmp ; j++)
            {
                box->buffer[pos%box->size] = ' ';
                pos++;
            }
        }
        else
        {
            box->buffer[pos%box->size] = s[i];
            pos++;
        }
    }
    tmp = (int)((int)(pos+(pos%box->size != 0)*box->w)/box->w)*box->w - pos;
    for(j=0 ; j<tmp ; j++)
    {
        box->buffer[pos%box->size] = ' ';
        pos++;
    }

    if(pos < box->size)
    {
        for(i=0 ; i<box->h ; i++)
        {
            move_cursor(box->sy+i, box->sx);
            for(j=0 ; j<box->w ; j++)
            {
                printf("%c", box->buffer[(i*box->w + j) % box->size]); 
                fflush(stdout);
            }
        }
    }
    else 
    {
        for(i=0 ; i<box->h ; i++)
        {
            move_cursor(box->sy+i, box->sx);
            for(j=0 ; j<box->w ; j++)
            {
                printf("%c", box->buffer[(pos + i*box->w + j) % box->size]); 
                fflush(stdout);
            }
        }
    }
    box->sp = pos;

	move_cursor_cmd();

}

void printfBox(struct printableBox* box, char* s, ...)
{
    char buf[1000];
    int size;
	va_list ap;

    memset(buf, 0, 1000);

	va_start(ap, s);
    vsprintf(buf, s, ap);
    size = strlen(buf);
    printToBox(box, buf, size);
	va_end(ap);
}

void init_print(struct printableBox* lobby, struct printableBox* chat)
{
}

void clear_screen()
{
    printf("\033[2J");
    printf("\033[H");
    fflush(stdout);
}

void update_screen(struct printableBox* lobby, struct printableBox* chat)
{
    clear_screen();
    print_grid();
    printToBox(lobby, "", 0);
    printToBox(chat, "", 0);
    if(cur_room_id != -1)
        print_room_title("ROOM %d", cur_room_id);
}

void print_room_title(char* s, ...)
{
    char buf[81];
    int size;
    va_list ap;

    memset(buf, 0, 81); 

    va_start(ap, s);
    vsprintf(buf, s, ap);
    size = strlen(buf);

    move_cursor(3, 80-size/2);
    printf("%s", buf);
    fflush(stdout);
    va_end(ap);
}

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
    printf("|  /ul = Show User Status                                                                                              |\n");
    printf("|  /rl = Show Room List                                                                                                |\n");
    printf("|  ---Room Command---                                                                                                  |\n");
    printf("|  !exit = Exit the room.                                                                                              |\n");
    printf("|  !emoji [EmojiName] = Send Emoji.                                                                                    |\n");
    printf("|  !file [Filepath] = Send File profile to Server.                                                                     |\n");
    printf("|                                                                                                                      |\n");
    printf("|  Press ENTER to exit...                                                                                              |\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n"); 
    fflush(stdout);
    char enter = 0;
    while (enter != '\r' && enter != '\n') { enter = getchar(); }
    update_screen(lobby, chat);
}

void print_grid()
{
    clear_screen();
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    printf("|                                      |                                                                               |\n");
    printf("|                LOBBY                 |                                                                               |\n");
    printf("|                                      |                                                                               |\n");
    printf("+--------------------------------------+-------------------------------------------------------------------------------|\n");
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
    printf("+----------------------------------------------------------------------------------------------------------------------|\n");
    printf("| >                                                                                                                    |\n");
    printf("+----------------------------------------------------------------------------------------------------------------------+\n");
    fflush(stdout);
}

void clear_box_buffer(struct printableBox* box)
{
    memset(box->buffer, 0, box->size);
    box->sp = 0;
    update_screen(lobby, chat);
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

void printfSplash(char* s, ...)
{
    va_list ap;

    va_start(ap, s);
    move_cursor(26, 3);
    for(int i=0 ; i<=114 ; i++) printf(" ");
    move_cursor(26, 3);
    vprintf(s, ap); 
    va_end(ap);
}