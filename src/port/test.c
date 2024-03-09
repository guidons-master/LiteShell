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
    return ch;
}

void _putchar(char c) {
    putchar(c);
    fflush(stdout);
}