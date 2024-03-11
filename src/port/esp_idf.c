#include <stdio.h>
#include "port.h"

// Note: ESP-IDF should disable buffering for stdout, use setvbuf(stdout, NULL, _IONBF, 0);

int _getchar() {
    return getchar();
}

void _putchar(char c) {
    putchar(c);
}