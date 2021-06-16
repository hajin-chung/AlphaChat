#include "../src/print.h"
#include <string.h>
#include <stdio.h>

char input[100];

int main()
{
    int size;
    int i;

    clear_screen();
    struct printableBox* lobby = initPrintableBox(5, 1, 10, 20);  

    while(1)
    {
        move_cursor(0, 0);
        fgets(input, 100, stdin);
        size = strlen(input);
    
        printToBox(lobby, input, size);
    }
}