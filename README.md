<div align="center">

<img src="./assets/liteshell.jpg" width="200" height="200" alt="LiteShell" />

# *LiteShell*
        
![Build Status](https://github.com/guidons-master/LiteShell/actions/workflows/build.yml/badge.svg)
[![GitHub license](https://img.shields.io/github/license/guidons-master/LiteShell)]()

</div>

## 项目介绍

**`LiteShell`** 是一个**轻量高效**、**简单易用**的嵌入式命令行工具，可以非常方便的嵌入到任何项目中，仅依赖`stdlib`标准库。

## 实现原理

命令注册和查找通过`hash-map` + `List`实现，可以在`O(1)`时间内完成查询。命令参数解析使用`有限状态机`实现，可以对命令参数进行高效解析。

## 使用说明

基本类型的参数签名定义如下:

| 类型                    | 签名 |
| ----------------------- | ---- |
| char(字符)              | c    |
| short、int、long(数字)  | i    |
| float(单精度浮点数)     | f    |
| double(双精度浮点数)    | d    |
| char*(字符串)           | s    |

参考示例代码:

```c
#include "liteshell.h"
#include <stdio.h>

// test命令的参数为空
void test() {
    Shell.print("Hello, world!\n");
}

// test1命令的参数为int、char、str、float、double
void test1(any_t a, any_t b, any_t c, any_t d, any_t e) {
    printf("a=%d, b=%c, c=%s, d=%f, e=%lf\n", a.i, b.c, c.str, d.f, e.d);
}

int main() {
    // 初始化Shell
    Shell.init();
    // 注册test命令, 参数为空
    Shell.add(FUNC_CONVERT(test, "", "test for print"));
    // 注册test1命令, 签名为"icsfd"
    Shell.add(FUNC_CONVERT(test1, "icsfd", "test1(int, char, char*, float, double)"));
    // 运行Shell
    while (1) Shell.run();
    // 销毁ShellQ
    Shell.free();
    return 0;
}
```

编译和运行示例代码:

```bash
gcc examples/basic.c src/liteshell.c src/port/test.c -Iinclude -o basic
./basic
```

Linux平台下运行效果如下:

![](./assets/demo.png)