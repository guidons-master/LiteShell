#include "liteshell.h"
#include <stdio.h>

struct test_t {
    int data;
} a = {0};

// test命令的参数为空
void test() {
    Shell.print("Hello, World!\n");
}

// test1命令的参数为int、char、str、float、double
int test1(any_t a, any_t b, any_t c, any_t d, any_t e) {
    printf("a=%d, b=%c, c=%s, d=%f, e=%lf\n", a.i, b.c, c.str, d.f, e.d);
    return 0;
}

// test2命令的参数为struct test_t*、void(*)()
void test2(struct test_t* a, any_t b) {
    printf("a=%d\n", a->data);
    ((void(*)())b.ptr)();
}

int main() {
    // 初始化Shell
    Shell.init();
    // 注册test命令, 参数为空
    Shell.addFunc(test, "", "test for print");
    // 注册test1命令, 签名为"icsfd"
    Shell.addFunc(test1, "icsfd", "test1(int, char, char*, float, double)");
    // 注册a变量，注册后可以在Shell中作为参数使用
    Shell.addVar(a, "test struct");
    // 注册test2命令, 签名为"pp"
    Shell.addFunc(test2, "pp", "test2(struct test_t*, void(*)())");
    // 运行Shell
    while (1) Shell.run();
    // 销毁Shell
    Shell.free();
    return 0;
}