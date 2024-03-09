#include "liteshell.h"
#include <stdio.h>

// test命令的参数为空
void test() {
    Shell.print("Hello, World!\n");
}

// test1命令的参数为int、char、str、float、double
void test1(any_t a, any_t b, any_t c, any_t d, any_t e) {
    printf("a=%d, b=%c, c=%s, d=%f, e=%lf\n", a.i, b.c, c.str, d.f, e.d);
}

int main() {
    // 初始化Shell
    Shell.init();
    // 注册test命令, 参数为空
    Shell.add(test, "", "test for print");
    // 注册test1命令, 签名为"icsfd"
    Shell.add(test1, "icsfd", "test1(int, char, char*, float, double)");
    // 运行Shell
    while (1) Shell.run();
    // 销毁Shell
    Shell.free();
    return 0;
}