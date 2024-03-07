#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "port/test.h"

int _getchar() {
    static struct termios old, new;
    static int count = 0;
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    static char ch;
    read(STDIN_FILENO, &ch, 1);
    if (ch == '\033') {
        count++;
    } else if (count == 0) {
        printf("%c", ch);
        fflush(stdout);    
    } else if (count != 0)
        count += 1;
    else if (count == 3)
        count = 0;
    
    return ch;
}

// int _getchar() {
//     return getchar();
// }

void _putchar(char c) {
    putchar(c);
    fflush(stdout);
}