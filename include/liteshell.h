#ifndef __LITESHELL_H__
#define __LITESHELL_H__

#if defined(__GNUC__) || defined(__clang__) || defined(__ICC) || defined(__ARMCC_VERSION)
    #define INLINE inline
#elif defined(_MSC_VER) || defined(__CC_ARM) || defined(__IAR_SYSTEMS_ICC__)
    #define INLINE __inline
#elif defined(__C51__) || defined(__CX51__)
    #define INLINE
#else
    #define INLINE
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define PACKED __attribute__((packed))
#elif defined(__ICCARM__) || defined(__CC_ARM)
    #define PACKED __packed
#else
    #define PACKED
#endif

/* 输入命令最大长度 */
#define CMD_LEN_MAX                           64U

/* 是否使用历史记录 */
#define USE_HISTORY

#ifdef USE_HISTORY
    /* 历史记录最大长度 */
    #define HISTORY_LEN_MAX                   10U
#endif

#define INITIAL_MAP_SIZE                      10U
#define PARAMS_MAX                            10U

#define addFunc(func, sign, desc)      export((func_t)func, #func, sign, desc)
#define addVar(var, desc)              export((func_t)0, #var, (void*)&var, desc)

typedef void (*func_t) ();

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

typedef struct PACKED {
    char _buff[CMD_LEN_MAX+1];
    void (*init)();
    void (*print)(const char*);
    void (*export)(func_t, const char*, const char*, const char*);
    void (*run)();
    void (*free)();  
} liteshell_t;

void _begin();
void _export(func_t, const char*, const char*, const char*);
void _run();
void _free();
void _print(const char*);
extern liteshell_t Shell;

#endif