#include "utils.h"
#include <stdio.h>

int main()
{
    printf("\033[0;0H");
    printf("1234");
    printf("\033[0;0H");
    printf("33");
}
