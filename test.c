#include <stdio.h>

struct test {
    int data;
} a = {1};

typedef union {
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int i;
    unsigned int ui;
    long l;
    float f;
    double d;
    char* str;
    void* ptr;
} any_t;

void test(any_t a) {
    printf("%d\n", ((struct test*)a.ptr)->data);
}

int main() {
    any_t p[2] = {0};
    p[0].ptr = &a;
    void (*func)(void) = test;
    ((void (*)(any_t))func)(p[0]);
    return 0;
}